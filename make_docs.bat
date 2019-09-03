@echo off
mkdocs build -q
set /p _="<html><head><meta http-equiv=refresh content="0; url=Docs/index.html"></head></html>" <nul > index.html
ver > nul