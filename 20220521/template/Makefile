obj-$(CONFIG_RACCOON_TEMPLATE)		+= rcn_template.o
rcn_template-objs			:= rcn_template_main.o

ifeq ($(CONFIG_DEBUG_FS),y)
rcn_template-objs			+= rcn_template_debugfs.o
endif
