#!/bin/sh

git_tag=$(git tag --sort=-v:refname | head -n1)
echo "${git_tag:-untagged}"
