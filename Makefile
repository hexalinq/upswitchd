GIT_COMMIT_HASH:=$(shell git log --max-count=1 --format="%H")
CFLAGS:=-Ofast -mtune=native -march=native -DPROGRAM_VERSION_STR="\"git-$(GIT_COMMIT_HASH)\""
LDFLAGS:=

upswitchd: obj/main.o
	gcc $(LDFLAGS) -o$@ $^

obj/%.o: src/%.c include/%.h
	@mkdir -p obj/
	gcc -c $(CFLAGS) -o$@ $<
