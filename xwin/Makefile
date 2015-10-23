XWIN_OBJ = xwin.o
XWIN_LIB = libxwin.so
TEST_XWIN = test_xwin

CFLAGS += -Wall

all: $(XWIN_LIB) $(TEST_XWIN)

$(XWIN_OBJ): xwin.h

$(XWIN_LIB): $(XWIN_OBJ)
	$(CC) -fPIC $(CFLAGS) $(XWIN_OBJ) -o $@ -shared

$(TEST_XWIN): main.o $(XWIN_LIB)
	$(CC) $(CFLAGS) -L. main.o -o $@ -lxwin -lX11

clean:
	rm -f *.o $(XWIN_LIB) $(TEST_XWIN)

