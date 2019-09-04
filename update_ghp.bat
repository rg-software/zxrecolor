if "%1" == "skip" goto skip
copy /y update_ghp.bat "%TEMP%"\update_ghp.bat 
"%TEMP%"\update_ghp.bat skip
:skip
hg up default
call make_docs.bat
del index.html
hg up gh-pages
robocopy Docs/ .  /s /move
hg addremove *
hg commit -m "Documentation update"
rem hg bookmark -r gh-pages gh-pages
hg up default