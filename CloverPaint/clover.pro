TEMPLATE	= app
CONFIG		= qt warn_on release
HEADERS		= blur2_normal.h \
		  blur2_reverse.h \
		  blur_normal.h \
		  blur_reverse.h \
		  brush2_normal.h \
		  brush2_reverse.h \
		  brush_normal.h \
		  brush_reverse.h \
		  brushsize11.h \
		  brushsize17.h \
		  brushsize25.h \
		  brushsize33.h \
		  brushsize65.h \
		  brushsize99.h \
		  clover.h \
		  clovericon.h \
		  cloverimage.h \
		  div255.h \
		  divInv360.h \
		  eraser2_normal.h \
		  eraser2_reverse.h \
		  eraser_normal.h \
		  eraser_reverse.h \
		  eraserimage.h \
		  filemenu_normal.h \
		  filemenu_reverse.h \
		  fill_normal.h \
		  fill_reverse.h \
		  hand.h \
		  hue.h \
		  layer_bottom_background.h \
		  layer_bottom_normal.h \
		  layer_bottom_reverse.h \
		  layer_free.h \
		  layer_hogo.h \
		  layer_invisible.h \
		  layer_nomode.h \
		  layer_normal.h \
		  layer_reverse.h \
		  layer_top.h \
		  layer_top_normal.h \
		  layer_top_reverse.h \
		  layer_visible.h \
		  lupe_normal.h \
		  lupe_normal2.h \
		  lupe_reverse.h \
		  lupe_reverse2.h \
		  mesh_normal.h \
		  mesh_reverse.h \
		  mode_normal.h \
		  mode_reverse.h \
		  palette_normal.h \
		  palette_reverse.h \
		  pencil_normal.h \
		  pencil_reverse.h \
		  penimage.h \
		  pensize_normal.h \
		  pensize_reverse.h \
		  scale_normal.h \
		  scale_reverse.h \
		  selectHide.h \
		  select_center.h \
		  select_normal.h \
		  select_reverse.h \
		  sharpen_normal.h \
		  sharpen_reverse.h \
		  tool_normal.h \
		  tool_reverse.h \
		  toolbarimage.h \
		  toolbarimage2_normal.h \
		  toolbarimage2_reverse.h \
		  toolbarimage_reverse.h
SOURCES		= canvas.cpp \
		  clover.cpp \
		  drawmode.cpp \
		  filemenu.cpp \
		  hue.cpp \
		  layer.cpp \
		  layermode.cpp \
		  lupe.cpp \
		  main.cpp \
		  mesh.cpp \
		  mytoolbar.cpp \
		  palette.cpp \
		  pensize.cpp \
		  scale.cpp \
		  select.cpp \
		  tool.cpp
INTERFACES	= 
DESTDIR = ./
INCLUDEPATH += $(QTDIR)/library
DEPENDPATH += $(QTDIR)/library
TARGET = cloverpaint
LIBS += -lqpe -lsl
