module:
	env/bin/pip install -e .

standalone: src/kelvandor/c/c.c
	cc -D STANDALONE -Wall src/kelvandor/c/c.c -o standalone
