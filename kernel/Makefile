DEFAULT_HOST!=../default-host.sh # elf-i686
HOST?=DEFAULT_HOST
HOSTARCH!=../target-triplet-to-arch.sh $(HOST)


CFLAGS?=-O2 -g -w
CPPFLAGS?=-w
LDFLAGS?=
LIBS?=

DESTDIR?=
PREFIX?=/usr/local
EXEC_PREFIX?=$(PREFIX)
BOOTDIR?=$(EXEC_PREFIX)/boot
INCLUDEDIR?=$(PREFIX)/include

CFLAGS:=$(CFLAGS) -ffreestanding -w -Wextra  -fno-asynchronous-unwind-tables
CPPFLAGS:=$(CPPFLAGS) -D__is_kernel -Iinclude
LDFLAGS:=$(LDFLAGS)
LIBS:=$(LIBS) -nostdlib -lk -lgcc # 指定 libk libgcc 内核专用数据结构 libk.a libgcc.a

ARCHDIR=arch/$(HOSTARCH)

include $(ARCHDIR)/make.config  # 读取架构相关的配置

CFLAGS:=$(CFLAGS) $(KERNEL_ARCH_CFLAGS)
CPPFLAGS:=$(CPPFLAGS) $(KERNEL_ARCH_CPPFLAGS)
LDFLAGS:=$(LDFLAGS) $(KERNEL_ARCH_LDFLAGS)
LIBS:=$(LIBS) $(KERNEL_ARCH_LIBS)   

KERNEL_OBJS=\
$(KERNEL_ARCH_OBJS) \
kernel/kernel.o \
kernel/segment.o kernel/interrupt.o \
kernel/printk.o kernel/setup.o \
kernel/mm_init.o kernel/page_alloc.o\
kernel/slab.o kernel/slub.o

OBJS=\
$(ARCHDIR)/crti.o \
$(ARCHDIR)/crtbegin.o \
$(KERNEL_OBJS) \
$(ARCHDIR)/crtend.o \
$(ARCHDIR)/crtn.o \

LINK_LIST=\
$(LDFLAGS) \
$(ARCHDIR)/crti.o \
$(ARCHDIR)/crtbegin.o \
$(KERNEL_OBJS) \
$(LIBS) \
$(ARCHDIR)/crtend.o \
$(ARCHDIR)/crtn.o \

.PHONY: all clean install install-headers install-kernel
.SUFFIXES: .o .c .S

all: sf-os.kernel

sf-os.kernel: $(OBJS) $(ARCHDIR)/linker.ld
	$(CC)   -T $(ARCHDIR)/linker.ld -o $@ $(CFLAGS) $(LINK_LIST)
	grub-file --is-x86-multiboot sf-os.kernel

$(ARCHDIR)/crtbegin.o $(ARCHDIR)/crtend.o:
	OBJ=`$(CC) $(CFLAGS) $(LDFLAGS) -print-file-name=$(@F)` && cp "$$OBJ" $@

.c.o:
	$(CC) -MD -c $< -o $@ -std=gnu11 $(CFLAGS) $(CPPFLAGS)

.S.o:
	$(CC) -D__ASM__ -MD -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

clean:
	rm -f sf-os.kernel
	rm -f $(OBJS) *.o */*.o */*/*.o
	rm -f $(OBJS:.o=.d) *.d */*.d */*/*.d

install: install-headers install-kernel

install-headers:
	mkdir -p $(DESTDIR)$(INCLUDEDIR)
	cp -R --preserve=timestamps include/. $(DESTDIR)$(INCLUDEDIR)/.

install-kernel: sf-os.kernel
	mkdir -p $(DESTDIR)$(BOOTDIR)
	cp sf-os.kernel $(DESTDIR)$(BOOTDIR)

-include $(OBJS:.o=.d)
