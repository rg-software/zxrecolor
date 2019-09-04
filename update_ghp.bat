hg up default
call make_docs.bat
del index.html
hg up gh-pages
robocopy Docs/ .  /s /move
hg addremove *
hg commit -m "Documentation update"
hg up default