#include <stdio.h>
#include <stdlib.h>

#include <qmainwindow.h>
#include <qwidget.h>
#include <qdirectpainter_qws.h>
#include <qapplication.h>
#include <qmessagebox.h>
#include <qimage.h>
#include <qfileinfo.h>
#include <qdialog.h>
#include <qbutton.h>

#include <qpe/qcopenvelope_qws.h>

#include <qpe/global.h>

#include <qpe/applnk.h>
#include <qpe/qpeapplication.h>


#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 640

#define TATE 0
#define YOKO 1

#define COLORDIV 4000

//ツールの種別
#define PEN 0
#define ERASER 1
#define BLUR 2
#define SHARPEN 3


//メッシュの種別
#define MY_BLACK 0
#define MY_DARK_GRAY 1
#define MY_GRAY 2
#define MY_FENCE_GRAY 3
#define MY_GRID_GRAY 4
#define MY_LIGHT_GRAY 5
#define MY_LEFT_INCLINE 6
#define MY_RIGHT_INCLINE 7
#define MY_BORDER 8
#define MY_STRIPE 9

// 背景色の描画
#define NORMAL_DRAW 100 //なし
#define OVERWRITE 200 //あり


//Y1はツールバーが下端のときの位置
//Y2はツールバーが上端のときの位置
#define PENSIZEPANEL_X 83
#define PENSIZEPANEL_Y1 362
#define PENSIZEPANEL_Y2 51

#define LAYERPANEL_X 16
#define LAYERPANEL_Y1 492
#define LAYERPANEL_Y2 51
#define LAYERICON_X 1
#define LAYERICON_Y 2
#define LAYERICON_WIDTH 29
#define LAYERICON_HEIGHT 29

#define DRAWMODEPANEL_X 49
#define DRAWMODEPANEL_Y2 81

#define MENUICON_X 1
#define MENUICON_Y 31
#define MENUICON_WIDTH 29
#define MENUICON_HEIGHT 29


#define PALETTEPANEL_X 415
#define PALETTEPANEL_Y1 359
#define PALETTEPANEL_Y2 51


#define MESHPANEL_X 346
#define MESHPANEL_Y1 359
#define MESHPANEL_Y2 51
#define TOOLBAR_Y1 558
#define TOOLBAR_Y2 20

#define HUEBAR_Y1 620
#define HUEBAR_Y2 0

#define SCALEPANEL_X 141
#define SCALEPANEL_Y1 449
#define SCALEPANEL_Y2 51

#define TOOLPANEL_X 83
#define TOOLPANEL_Y1 449
#define TOOLPANEL_Y2 80

#define TOOLICON_X 68
#define TOOLICON_Y 31
#define TOOLICON_WIDTH 29
#define TOOLICON_HEIGHT 29


#define SELECTPANEL_X 112
#define SELECTPANEL_Y2 81

#define LUPEPANEL_X 254
#define LUPEPANEL_Y1 313
#define LUPEPANEL_Y2 81
#define LUPE_HEIGHT 223 // 描画領域の高さ
#define LUPE_WIDTH 223


// zoomStyle
#define NORMAL 0
#define ZOOM 1
#define LUPE 2

#define HANDICON_X 97
#define HANDICON_Y 2
#define HANDICON_WIDTH 29
#define HANDICON_HEIGHT 29


#define SCALEICON_X 126
#define SCALEICON_Y 2
#define SCALEICON_WIDTH 29
#define SCALEICON_HEIGHT 29

#define SELECTICON_X 97
#define SELECTICON_Y 31
#define SELECTICON_WIDTH 29
#define SELECTICON_HEIGHT 29

#define LUPEICON_X 126
#define LUPEICON_Y 31
#define LUPEICON_WIDTH 29
#define LUPEICON_HEIGHT 29

#define FILEMENUICON_X 1
#define FILEMENUICON_Y 31
#define FILEMENUICON_WIDTH 29
#define FILEMENUICON_HEIGHT 29
#define FILEMENUPANEL_Y2 81

#define SPOIT_X 362
#define SPOIT_Y 2
#define SPOIT_WIDTH 29
#define SPOIT_HEIGHT 29

#define UNDO_X 449
#define UNDO_Y 2
#define UNDO_WIDTH 29
#define UNDO_HEIGHT 29



#define COLORAREA_X 393
#define COLORAREA_Y 2
#define COLORAREA_WIDTH 25
#define COLORAREA_HEIGHT 23

#define COLORAREA2_X 399
#define COLORAREA2_Y 10
#define COLORAREA2_WIDTH 25
#define COLORAREA2_HEIGHT 23

#define COLORAREA3_X 426
#define COLORAREA3_Y 2
#define COLORAREA3_WIDTH 21
#define COLORAREA3_HEIGHT 31

#define SBAREA_X 158
#define SBAREA_Y 2
#define SBAREA_WIDTH 170
#define SBAREA_HEIGHT 58


#define NORMAL 0
#define MULTIPLE 1
#define SCREEN 2
#define ADD 3
#define LIGHTEN 4
#define DARKEN 5
#define DIFFERENT 6
#define HUE 7
#define SATURATION 8
#define COLOR 9
#define BRIGHTNESS 10
#define BURN 11
#define DODGE 12
#define MASK 13



#define VGA 0
#define QVGA 1



class Canvas;
class MyToolBar;
class PenSize;
class Mesh;
class Palette;
class Scale;
class Tool;
class Lupe;
class Hue;
class Layer;
class LayerMode;
class DrawMode;
class FileMenu;
class Select;


class CloverPaint : public QMainWindow {
  Q_OBJECT

public:
  CloverPaint();
  ~CloverPaint();

  virtual void resizeEvent(QResizeEvent *e);
  virtual void keyPressEvent( QKeyEvent *e );
  virtual void keyReleaseEvent( QKeyEvent *e );
  virtual void closeEvent( QCloseEvent *e );

  //  virtual bool event(QEvent *e);

  Canvas *canvas;

  void openImage();
  void loadLayer();
  void saveLayer();
  void loadCpfImage(QString filename);
  void savePngImage();
  void saveJpgImage();
  void saveCpfImage();
  void saveImage();

  void myResize();
  
  //  bool activateFlag;

private:

  bool filedialogOn;
  bool initFlag;
  void resize();

};


class Canvas : public QWidget {
  Q_OBJECT

public:
  Canvas(CloverPaint *parent);
  ~Canvas();
  void changeToolbarVisible();
  void showToolbar1();

  void RGB2HSB();
  void HSB2RGB();
  void HSB2RGBd();
  void CalcHue(unsigned int *tmpR1, unsigned int *tmpG1, unsigned int *tmpB1, unsigned int tmpR2, unsigned int tmpG2, unsigned int tmpB2);
  void CalcBrightness(unsigned int *tmpR1, unsigned int *tmpG1, unsigned int *tmpB1, unsigned int tmpR2, unsigned int tmpG2, unsigned int tmpB2);
  void CalcColor(unsigned int *tmpR1, unsigned int *tmpG1, unsigned int *tmpB1, unsigned int tmpR2, unsigned int tmpG2, unsigned int tmpB2);
  void CalcSaturation(unsigned int *tmpR1, unsigned int *tmpG1, unsigned int *tmpB1, unsigned int tmpR2, unsigned int tmpG2, unsigned int tmpB2);

  virtual void paintEvent( QPaintEvent *e );
  virtual void mousePressEvent( QMouseEvent *e );
  virtual void mouseReleaseEvent( QMouseEvent *e );
  virtual void mouseMoveEvent( QMouseEvent *e );

  uchar colorMatrix[3][256];
  bool useColorMatrix;

  struct BUFSTR {
    int sx;    /* 領域右端のX座標 */
    int sy;    /* 領域のY座標 */
  };
  struct BUFSTR Buffer[1024];
  struct BUFSTR *Start_Idx, *End_Idx;

  bool dispLupeFlag;

  bool bokeashiFlag;
  bool kasanenuriFlag;

  bool penDownFlag;
  bool penPressFlag;

  bool powerBlurFlag;

  unsigned int calc(short command, unsigned int ope1, unsigned int ope2, int layeralpha);

  unsigned int pencolor;
  unsigned int bgcolor;

  unsigned int penalpha;
  unsigned int penalpha2;

  unsigned int eraseralpha;
  unsigned int eraseralpha2;

  unsigned int bluralpha;
  unsigned int bluralpha2;


  unsigned int sharpenalpha;
  unsigned int sharpenalpha2;

  char canvasType;

  bool fillFlag;

  char windowDirection;

  unsigned int fgHu;  
  unsigned int fgSa;  
  unsigned int fgBr;  

  double fgHu2;
  double fgSa2;
  double fgBr2;

  QImage *bgImage;  

  QImage *brushImage;

  QImage *brush11;
  QImage *brush17;
  QImage *brush25;
  QImage *brush33;
  QImage *brush65;
  QImage *brush99;

  QImage *layer[5];  
  char layerMode[5]; 
  char drawMode; 
  char previousLayerMode;
  bool layerVisible[5];  
  bool layerDrawable[5];  
  QString layerName[5];
  short layerNameCounter;
  unsigned char layerAlpha[5];
  // 0 - 256 まで．
  unsigned short layerAlpha2[5];

  QString previousDirectory;
  QString currentFilename;

  unsigned int *drawChecker;
  unsigned int *redoBuffer;
  unsigned int *undoBuffer;

  Q_INT32 colorPalette[10];


  int screen_width;
  int screen_height;

  short pensize;
  short erasersize;
  short blursize;
  short sharpensize;

  short tool;
  short meshpattern;
  short overwrite;
  short toolbarIndex;

  MyToolBar *toolbar;
  Hue *huebar;


  bool spoitSelectFlag;

  int toolbarY;
  int pensizePanelY;
  int meshPanelY;
  int palettePanelY;
  int scalePanelY;
  int toolPanelY;
  int selectPanelY;
  int huebarY;
  int layerPanelY;
  int drawModePanelY;
  int fileMenuPanelY;

  int moveRectX;
  int moveRectY;

  char layernum; // 全レイヤ数
  int currentlayer; //現在のレイヤ

  PenSize *pensizePanel;
  Mesh *meshPanel;
  Palette *palettePanel;
  Scale *scalePanel;
  Tool *toolPanel;
  Select *selectPanel;
  Lupe *lupePanel;
  Layer *layerPanel;
  LayerMode *layerModePanel;
  DrawMode *drawModePanel;
  FileMenu *fileMenuPanel;


  bool fileChangeFlag;

  CloverPaint *clover;

  int canvasTop;
  int canvasLeft;

  int zoomScale;
  int lastScale;
  int zoomTop;
  int zoomLeft;
  int lupeScale;
  int lupeTop;
  int lupeLeft;

  int zoomStyle;
  bool zoomSelectFlag;
  bool handFlag;

  bool spoitJogSelectFlag;
  bool spoitJogReleaseFlag;

  QImage *cloverImg;
  QImage *mode_normalImg;
  QImage *mode_reverseImg;

  QImage *pensize_normalImg;
  QImage *mesh_normalImg;
  QImage *palette_normalImg;
  QImage *pensize_reverseImg;
  QImage *mesh_reverseImg;
  QImage *palette_reverseImg;

  //  QImage *brushImg;
  //  QPixmap* brushPix;

  bool moveRectFlag;

  int drawLineMode;

  bool undoFlag;

  void drawLine(bool penDown, bool penUp);
  void repaintRect();
  int repaint_left;
  int repaint_top;
  int repaint_width;
  int repaint_height;

  int undo_left;
  int undo_top;
  int undo_right;
  int undo_bottom;

  QPoint *point, *prevpoint;
  QPoint *point2, *prevpoint2;

  bool repaintRectFlag;

  int blurOrSharpen;

  void fill(int x, int y);

  int drawPoint(int x, int y, unsigned int color, int brushAlpha, bool drawFlag);

protected:

private:


  void meshPoint(int x, int y, int brushAlpha, int toolAlpha);

  void scan_line(int lx, int rx, int y, unsigned int col);

};



class MyToolBar : public QDialog {
  Q_OBJECT

public:
  MyToolBar(Canvas * parent=0, const char * name=0, bool modal=FALSE, WFlags f=0);
  ~MyToolBar();
  virtual void paintEvent( QPaintEvent *e );
  virtual void mousePressEvent( QMouseEvent *e );
  virtual void mouseReleaseEvent( QMouseEvent *e );
  virtual void mouseMoveEvent( QMouseEvent *e );


  void repaintColor(int left, int top, int width, int height, bool repaintAll);
  void repaintLayerOpacity(int left, int top, int width, int height);


  void changeToEraser();
  void changeToDrawtool();
  void changeToBlur();
  void changeToSharpen();

  void redrawPenSize(int size);
  void redrawMesh();
  void redrawToolBar();

  void reverseSBCursor();

  void changeColorArea();

  void changeOpacityArea(int x);
  void changeOpacityArea2(int tmpOpacity);

  void changeSBArea();

  QImage toolbarImg;  
  QImage toolbar_normalImg;  
  QImage toolbar_reverseImg;
  QImage eraser_normalImg;  
  QImage eraser_reverseImg;  
  QImage eraser2_normalImg;  
  QImage eraser2_reverseImg;  
  QImage pencil_normalImg;  
  QImage pencil_reverseImg;
  QImage brush_normalImg;  
  QImage brush_reverseImg;  
  QImage brush2_normalImg;  
  QImage brush2_reverseImg;  
  QImage blur_normalImg;  
  QImage blur_reverseImg;
  QImage blur2_normalImg;  
  QImage blur2_reverseImg;
  QImage sharpen_normalImg;  
  QImage sharpen_reverseImg;
  QImage fill_normalImg;  
  QImage fill_reverseImg;


  QImage handImg;
  QImage selectHideImg;


  int repaint_left;
  int repaint_top;
  int repaint_width;
  int repaint_height;

  bool opacitySelectingFlag;
  bool opacityMinusSelectFlag;
  bool opacityPlusSelectFlag;

  bool sbSelectingFlag;



protected:

private:
  Canvas *canvas;

};



class Hue : public QDialog {
  Q_OBJECT

public:
  Hue(Canvas * parent=0, const char * name=0, bool modal=FALSE, WFlags f=0);
  ~Hue();

  virtual void paintEvent( QPaintEvent *e );
  virtual void mousePressEvent( QMouseEvent *e );
  virtual void mouseReleaseEvent( QMouseEvent *e );
  virtual void mouseMoveEvent( QMouseEvent *e );


  void changeHueArea(int hue, int huePoint);

  QImage hueImg;  
  
  int prevHuePoint;

  int repaint_left;
  int repaint_top;
  int repaint_width;
  int repaint_height;

protected:

private:
  Canvas *canvas;

};




class PenSize : public QDialog {
  Q_OBJECT

public:
  PenSize(Canvas * parent=0, const char * name=0, bool modal=FALSE, WFlags f=0);
  ~PenSize();


  virtual void paintEvent( QPaintEvent *e );
  virtual void mousePressEvent( QMouseEvent *e );
  virtual void mouseReleaseEvent( QMouseEvent *e );
  virtual void mouseMoveEvent( QMouseEvent *e );
  virtual void showEvent( QShowEvent * );
  virtual void hideEvent( QHideEvent * );

  void myReset();

  void changePenSize(int size);
  QImage *pensizeImg;  

  Canvas *canvas;
  //  MyToolBar *mytoolbar;
  
  bool display;

  bool stickyFlag;
  bool moveFlag;
  bool minimizePressFlag;
  bool closePressFlag;

  int prev_x;
  int prev_y;

protected:

private:


};



class Layer : public QDialog {
  Q_OBJECT

public:
  Layer(Canvas * parent=0, const char * name=0, bool modal=FALSE, WFlags f=0);
  ~Layer();

  bool display;

  virtual void mousePressEvent( QMouseEvent *e );
  virtual void mouseReleaseEvent( QMouseEvent *e );
  virtual void mouseMoveEvent( QMouseEvent *e );
  virtual void showEvent( QShowEvent * );
  virtual void hideEvent( QHideEvent * );

  void RebuildPanel();

  void refreshDirectPaintArea();
  void changeOpacityArea(int x);

  QImage *layerPanelImg;
  QImage *layer_normalImg;
  QImage *layer_normalImgOrg;
  QImage *layer_hogoImg;
  QImage *layer_freeImg;
  QImage *layer_nomodeImg;
  QImage *layer_visibleImg;
  QImage *layer_invisibleImg;
  QImage *layer_top_normalImg;
  QImage *layer_top_reverseImg;
  QImage *layer_bottom_normalImg;
  QImage *layer_bottom_reverseImg;
  QImage *layer_bottom_backgroundImg;
  
  Canvas *canvas;
  
  bool stickyFlag;
  bool moveFlag;
  bool minimizePressFlag;
  bool closePressFlag;

  bool opacitySelectingFlag;
  bool opacityMinusSelectFlag;
  bool opacityPlusSelectFlag;

  int prev_x;
  int prev_y;

  int repaint_left;
  int repaint_top;
  int repaint_width;
  int repaint_height;

protected:

private:

};



class Mesh : public QDialog {
  Q_OBJECT

public:
  Mesh(Canvas * parent=0, const char * name=0, bool modal=FALSE, WFlags f=0);
  ~Mesh();

  bool display;
  virtual void paintEvent( QPaintEvent *e );
  virtual void mousePressEvent( QMouseEvent *e );
  virtual void mouseReleaseEvent( QMouseEvent *e );
  virtual void mouseMoveEvent( QMouseEvent *e );
  virtual void showEvent( QShowEvent * );
  virtual void hideEvent( QHideEvent * );

  void changeMesh(int mesh, int overwrite);
  QImage *meshImg;  

  Canvas *canvas;
  //  MyToolBar *mytoolbar;
  
  bool stickyFlag;
  bool moveFlag;
  bool minimizePressFlag;
  bool closePressFlag;

  int prev_x;
  int prev_y;

protected:

private:


};



class Palette : public QDialog {
  Q_OBJECT

public:
  Palette(Canvas * parent=0, const char * name=0, bool modal=FALSE, WFlags f=0);
  ~Palette();

  bool display;

  virtual void mousePressEvent( QMouseEvent *e );
  virtual void mouseReleaseEvent( QMouseEvent *e );
  virtual void mouseMoveEvent( QMouseEvent *e );
  virtual void showEvent( QShowEvent * );

  void myReset();

  QImage *paletteImg;  

  Canvas *canvas;


  bool paletteSpoit;
  
  bool stickyFlag;
  bool moveFlag;
  bool minimizePressFlag;
  bool closePressFlag;

  int prev_x;
  int prev_y;

protected:

private:


};




class Scale : public QDialog {
  Q_OBJECT

public:
  Scale(Canvas * parent=0, const char * name=0, bool modal=FALSE, WFlags f=0);
  ~Scale();

  bool display;
  virtual void paintEvent( QPaintEvent *e );
  virtual void mousePressEvent( QMouseEvent *e );
  virtual void mouseReleaseEvent( QMouseEvent *e );
  virtual void mouseMoveEvent( QMouseEvent *e );
  virtual void showEvent( QShowEvent * );
  virtual void hideEvent( QHideEvent * );

  void myReset();

  void changeScale(int scale, int select_center);
  void changeOffset(int scale);
  QImage scale_normalImg;  
  QImage scale_reverseImg;  
  QImage scaleImg;  
  QImage select_centerImg;

  Canvas *canvas;
  
  bool stickyFlag;
  bool moveFlag;
  bool minimizePressFlag;
  bool closePressFlag;
  int prev_x;
  int prev_y;
  
protected:

private:


};


class Tool : public QDialog {
  Q_OBJECT

public:
  Tool(Canvas * parent=0, const char * name=0, bool modal=FALSE, WFlags f=0);
  ~Tool();

  bool display;
  virtual void paintEvent( QPaintEvent *e );
  virtual void mousePressEvent( QMouseEvent *e );
  virtual void mouseReleaseEvent( QMouseEvent *e );
  virtual void mouseMoveEvent( QMouseEvent *e );
  virtual void showEvent( QShowEvent * );
  virtual void hideEvent( QHideEvent * );
  
  void changeTool(int button_x, int button_y);
  void myReset();

  QImage tool_normalImg;  
  QImage tool_reverseImg;  
  QImage toolImg;  

  Canvas *canvas;
  
  bool stickyFlag;
  bool moveFlag;
  bool minimizePressFlag;
  bool closePressFlag;
  int prev_x;
  int prev_y;
  
protected:

private:


};


class Select : public QDialog {
  Q_OBJECT

public:
  Select(Canvas * parent=0, const char * name=0, bool modal=FALSE, WFlags f=0);
  ~Select();

  bool display;
  virtual void paintEvent( QPaintEvent *e );
  virtual void mousePressEvent( QMouseEvent *e );
  virtual void mouseReleaseEvent( QMouseEvent *e );
  virtual void mouseMoveEvent( QMouseEvent *e );
  virtual void showEvent( QShowEvent * );
  virtual void hideEvent( QHideEvent * );

  void myReset();

  QImage select_normalImg;  
  QImage select_reverseImg;  
  QImage selectImg;  

  Canvas *canvas;
  
  bool stickyFlag;
  bool moveFlag;
  bool minimizePressFlag;
  bool closePressFlag;
  int prev_x;
  int prev_y;
  
protected:

private:


};




class Lupe : public QDialog {
  Q_OBJECT

public:
  Lupe(Canvas * parent=0, const char * name=0, bool modal=FALSE, WFlags f=0);
  ~Lupe();

  bool display;

  virtual void showEvent( QShowEvent * );
  virtual void hideEvent( QHideEvent * );
  virtual void mousePressEvent( QMouseEvent *e );
  virtual void mouseReleaseEvent( QMouseEvent *e );
  virtual void mouseMoveEvent( QMouseEvent *e );

  void myReset();
  void changeWidget();

  QImage lupe_normalImg;  
  QImage lupe_reverseImg;  
  bool minimizePressFlag;
  bool closePressFlag;
  QImage lupeImg;  

  Canvas *canvas;

  bool moveFlag;
  int prev_x;
  int prev_y;

  bool drawFlag;

protected:

private:


};


class LayerMode : public QDialog {
  Q_OBJECT

public:
  LayerMode(Canvas * parent=0, const char * name=0, bool modal=FALSE, WFlags f=0);
  ~LayerMode();


  virtual void mousePressEvent( QMouseEvent *e );
  virtual void mouseReleaseEvent( QMouseEvent *e );
  virtual void mouseMoveEvent( QMouseEvent *e );
  virtual void showEvent( QShowEvent * );
  virtual void hideEvent( QHideEvent * );

  QImage modeImg;

  Canvas *canvas;


protected:

private:


};



class DrawMode : public QDialog {
  Q_OBJECT

public:
  DrawMode(Canvas * parent=0, const char * name=0, bool modal=FALSE, WFlags f=0);
  ~DrawMode();


  virtual void mousePressEvent( QMouseEvent *e );
  virtual void mouseReleaseEvent( QMouseEvent *e );
  virtual void mouseMoveEvent( QMouseEvent *e );
  virtual void showEvent( QShowEvent * );
  virtual void hideEvent( QHideEvent * );

  QImage modeImg;

  Canvas *canvas;


protected:

private:


};



class FileMenu : public QDialog {
  Q_OBJECT

public:
  FileMenu(Canvas * parent=0, const char * name=0, bool modal=FALSE, WFlags f=0);
  ~FileMenu();


  virtual void mousePressEvent( QMouseEvent *e );
  virtual void mouseReleaseEvent( QMouseEvent *e );
  virtual void mouseMoveEvent( QMouseEvent *e );
  virtual void showEvent( QShowEvent * );
  virtual void hideEvent( QHideEvent * );

  QImage filemenuImg;
  QImage filemenu_normalImg;  
  QImage filemenu_reverseImg;  

  Canvas *canvas;


protected:

private:


};

