@echo off 
if "%1" == "skip" goto skip
copy /y update_ghp.bat "%TEMP%"\update_ghp.bat 
"%TEMP%"\update_ghp.bat skip
:skip
hg up default
call make_docs.bat
del index.html
hg up docs
robocopy Docs/ .  /s /move
hg addremove *
hg commit -m "Documentation update"
hg bookmark -r docs gh-pages
hg up default
hg bookmark -r default master