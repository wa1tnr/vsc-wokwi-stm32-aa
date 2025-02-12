
git_status:
	@git status | cat | tr -d '\t'

pwd:
	@echo -n '  ..'
	@pwd | cut -b 27-

