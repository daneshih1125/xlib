LDFLAGS = -lX11

all:
	@for src in $(shell ls *.c); do \
		$(CC) -o $${src%.*} $$src $(LDFLAGS); \
	done

clean:
	@for src in $(shell ls *.c); do \
		rm -f $${src%.*}; \
	done
