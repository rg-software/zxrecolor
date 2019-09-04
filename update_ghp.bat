hg up default
call make_docs.bat
hg up gh-pages
robocopy Docs/ .  /s /move
hg addremove *
hg commit -m "Documentation update"
hg up default