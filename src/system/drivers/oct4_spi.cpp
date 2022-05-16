// oCt4drv -- Basic drivers for O_C + T4.0
// Copyright (C) 2022 Patrick Dowling (pld@gurkenkiste.com)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "system/drivers/oct4_spi.h"

#include "system/t4.h"
#include "system/system_serial.h"

// - Borrow the init functionality from <SPI.h> and assuem the clocks will be right
// - ...and also looks like it initialized the relevant pins with fast IO

namespace oct4 {
namespace drivers {

static DMAChannel dma_channel_spi_tx{false};

FLASHMEM void SPI::Init(uint32_t clock)
{
  ::SPI.setCS(PIN_DAC_CS);
  ::SPI.begin();
  ::SPI.beginTransaction({clock, MSBFIRST, SPI_MODE0});

  SERIAL_DEBUG("PIN_DAC_CS: %d", ::SPI.pinIsChipSelect(PIN_DAC_CS));
  SERIAL_DEBUG("spi.param=%08" PRIx32, IMXRT_LPSPI4_S.PARAM);

  dma_channel_spi_tx.begin(true);
  SERIAL_DEBUG("dma_channel_spi_tx.channel=%x", dma_channel_spi_tx.channel);
  dma_channel_spi_tx.disable();
  // NOTE To use a different data type, we'll need to adjust dma_channel_spi_tx.destination
  dma_channel_spi_tx.destination((volatile uint8_t &)IMXRT_LPSPI4_S.TDR);
  dma_channel_spi_tx.triggerAtHardwareEvent(DMAMUX_SOURCE_LPSPI4_TX);
  dma_channel_spi_tx.disableOnCompletion();

  // Default RXFIFO/TXFIFO in PARAM is 4 = 16, but we don't trust the defaults
  // TODO PCSNUM
  // IMXRT_LPSPI4_S.PARAM = 0x00000404;
  IMXRT_LPSPI4_S.FCR = LPSPI_FCR_TXWATER(15);

  tcr_ = IMXRT_LPSPI4_S.TCR;
  tcr_ |= LPSPI_TCR_RXMSK;  // ignore RX fifo, we're only transmitting anyway
  //  "When RXMSK is set, receive data is discarded instead of storing in the receive FIFO"

  SetMode<PCS_DISABLE, 8>();
}

void SPI::UpdateTCR(PCSMode pcs_mode, uint8_t frame_size)
{
  uint32_t tcr = tcr_;
  // There's the LPSPI_CFGR1_PCSCFG flag but that only works if the SPI is disabled.
  // Since we've only muxed one pin to PCS selecting a different oen should disable it.
  // TODO maybe there's a better way
  if (PCS_ENABLE == pcs_mode)
    tcr &= ~LPSPI_TCR_PCS(0xf);
  else
    tcr |= LPSPI_TCR_PCS(1);

  tcr &= ~LPSPI_TCR_FRAMESZ(0xfff);
  tcr |= LPSPI_TCR_FRAMESZ(frame_size - 1);
  tcr_ = tcr;
}

template <typename T>
static void Tx(uint32_t tcr, const T *data, size_t count, SPI::FlushMode flush_mode)
{
  IMXRT_LPSPI4_S.CR = LPSPI_CR_MEN | LPSPI_CR_RRF | LPSPI_CR_RTF;  // clear queues & enable
  IMXRT_LPSPI4_S.TCR = tcr;

  while (count--) {
    IMXRT_LPSPI4_S.TDR = *data++;
    while (!(IMXRT_LPSPI4_S.SR & LPSPI_SR_TDF)) {}  // TDF: words in FIFO <= FCR:TXWATER }
  }

  if (SPI::FLUSH == flush_mode) {
    while (IMXRT_LPSPI4_S.SR & LPSPI_SR_MBF) {}
  }
}

FASTRUN
void SPI::Send(const uint8_t *data, size_t num_bytes, FlushMode flush_mode)
{
  Tx(tcr_, data, num_bytes, flush_mode);
}

FASTRUN
void SPI::Send(const uint32_t *data, size_t count, FlushMode flush_mode)
{
  Tx(tcr_, data, count, flush_mode);
}

void SPI::Flush()
{
  // while (LPSPI_FSR_TXCOUNT(IMXRT_LPSPI4_S.FSR)) {}
  while (IMXRT_LPSPI4_S.SR & LPSPI_SR_MBF) {}
}

static constexpr uint32_t TCR_FLAGS_DMA = LPSPI_TCR_CONT;

void SPI::AsyncTransferBegin(const uint8_t *data, size_t num_bytes)
{
  // Flush cache here, something like the screen driver doesn't need to care.
  arm_dcache_flush_delete((void *)data, num_bytes);
  dma_channel_spi_tx.sourceBuffer(data, num_bytes);

  IMXRT_LPSPI4_S.CR = LPSPI_CR_MEN | LPSPI_CR_RRF | LPSPI_CR_RTF;  // clear queues & enable
  IMXRT_LPSPI4_S.DER = LPSPI_DER_TDDE;                             // enable TX DMA
  IMXRT_LPSPI4_S.TCR = tcr_ | TCR_FLAGS_DMA;  // update TCR with current settings
  dma_channel_spi_tx.enable();
  dma_active_ = true;
}

void SPI::AsyncTransferWait()
{
  if (dma_active_) {
    while (!dma_channel_spi_tx.complete()) {}
    IMXRT_LPSPI4_S.DER = 0;  // disable DMA
    IMXRT_LPSPI4_S.TCR = tcr_;
    Flush();

    dma_channel_spi_tx.clearComplete();
    dma_channel_spi_tx.disable();
    // dma_channel_spi_tx.clearError();
    dma_active_ = false;
  }
}

}  // namespace drivers
}  // namespace oct4
