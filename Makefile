LDFLAGS = -lX11 -lpng

all:
	@for src in $(shell ls *.c); do \
		$(CC) -Wall -o $${src%.*} $$src $(LDFLAGS); \
	done

clean:
	@for src in $(shell ls *.c); do \
		rm -f $${src%.*}; \
	done
