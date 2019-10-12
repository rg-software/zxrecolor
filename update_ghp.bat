@echo off 
:: Documentation updater
::
:: make sure there are no uncommitted changes in the project
:: before calling this script!

:: running the batch from an outside temp folder
if "%1" == "skip" goto skip
copy /y update_ghp.bat "%TEMP%"\update_ghp.bat 
"%TEMP%"\update_ghp.bat skip
:skip

:: update documentation branch, 
:: then switch back to the default
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