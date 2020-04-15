#include "clover.h"
#include <math.h>

#include "cloverimage.h"
#include "mode_normal.h"
#include "mode_reverse.h"

#include "pensize_normal.h"
#include "mesh_normal.h"
#include "palette_normal.h"
#include "pensize_reverse.h"
#include "mesh_reverse.h"
#include "palette_reverse.h"

#include "brushsize11.h"
#include "brushsize17.h"
#include "brushsize25.h"
#include "brushsize33.h"
#include "brushsize65.h"
#include "brushsize99.h"


#define MESHICON_X 331
#define MESHICON_Y 2
#define MESHICON_WIDTH 29
#define MESHICON_HEIGHT 29



Canvas::Canvas(CloverPaint *parent)
  :QWidget(parent)
{
  clover = parent;

  //canvasType = QVGA;
  canvasType = VGA;

  fileChangeFlag = false;

  undoFlag = false;

  drawLineMode = 0;

  dispLupeFlag = false;

  pensize = 1;
  erasersize = 1;
  blursize = 1;
  sharpensize = 1;

  bokeashiFlag = false;
  kasanenuriFlag = false;

  penDownFlag = false;
  penPressFlag = false;

  tool = PEN;
  meshpattern = MY_BLACK;
  overwrite = NORMAL_DRAW;

  layernum = 1;

  previousLayerMode = NORMAL;

  drawMode = NORMAL;
  
  fillFlag = false;

  currentlayer = 0;

  point = new QPoint(0,0);
  prevpoint = new QPoint(0,0);
  point2 = new QPoint(0,0);
  prevpoint2 = new QPoint(0,0);


  blurOrSharpen = BLUR;

  pencolor = 0xff000000;

  penalpha =100;
  penalpha2 =256;
  eraseralpha =100;
  eraseralpha2 =256;

  bluralpha =100;
  bluralpha2 =256;
  sharpenalpha =100;
  sharpenalpha2 =256;

  bgcolor = 0xffffffff;

  zoomScale = 0; //シフト数で指定。 0 は等倍．
  lastScale = 1; //最後に選択された等倍以上の拡大率
  lupeScale = 2; //シフト数で指定。 0 は等倍．
  zoomStyle = NORMAL;
  zoomTop = 120;
  zoomLeft = 160;
  lupeTop = 210 ;
  lupeLeft = 130;


  if(canvasType == QVGA){
    zoomScale = 1;
    lupeScale = 2;
    zoomStyle = ZOOM;
    lastScale = 2; //最後に選択された等倍以上の拡大率
    zoomTop = 0;
    zoomLeft = 0;
    lupeTop = 105+22;
    lupeLeft = 65+22;
  }

  canvasTop = 0;
  canvasLeft = 0;


  


  zoomSelectFlag = false;
  handFlag = false;
  spoitSelectFlag = false;
  spoitJogSelectFlag = false;

  spoitJogReleaseFlag = false;

  moveRectFlag = false;

  repaint_left = 0;
  repaint_top = 0;
  repaint_width = 480;
  repaint_height = 640;
  repaintRectFlag = false;

  undo_left = 480;
  undo_top = 640;
  undo_right = 480;
  undo_bottom = 640;


  fgHu = 0;
  fgSa = 0;
  fgBr = 0;

  fgHu2 = 0;
  fgSa2 = 0;
  fgBr2 = 0;

  //オフスクリーン作成(フレームバッファにあわせて必ず縦長で)


  bgImage = new QImage(SCREEN_WIDTH,
		       SCREEN_HEIGHT,
		       32,
		       0,
		       QImage::IgnoreEndian);
  //オフスクリーン初期化（白で塗りつぶし）
  for(int x=0; x<SCREEN_WIDTH; x++){
    for(int y=0; y<SCREEN_HEIGHT; y++){
      bgImage->setPixel(x,y,0xffffffff);
    }
  }

  layer[0] = new QImage();
  if(!layer[0]->create(SCREEN_WIDTH,
		       SCREEN_HEIGHT,
		       32,
		       0,
		       QImage::IgnoreEndian)){
    if (!QMessageBox::warning(this, "CloverPaint", "Sorry, no available memory...\n","OK")){
      clover->close();
      return;
    }
  }


  for(int x=0; x<SCREEN_WIDTH; x++){
    for(int y=0; y<SCREEN_HEIGHT; y++){
      layer[0]->setPixel(x,y,0xffffffff);
    }
  }

  layerMode[0] = NORMAL;
  layerVisible[0] = true;
  layerDrawable[0] = true;
  layerAlpha[0] = 100;
  layerAlpha2[0] = 256;

  for(int i=0; i<5; i++){
    layerMode[i] = NORMAL;
    layerVisible[i] = true;
    layerDrawable[i] = true;
    layerAlpha[i] = 100;
    layerAlpha2[i] = 256;
  }
  layerName[0] = "Background";
  layerName[1] = "Layer  1";
  layerName[2] = "Layer  2";
  layerName[3] = "Layer  3";
  layerName[4] = "Layer  4";

  //レイヤの命名に利用
  layerNameCounter = 1;

  //前回選択したディレクトリ名
  previousDirectory = "/home/zaurus/Documents/Image_Files/";
  currentFilename =  "";

  //Undoバッファ
  undoBuffer = (unsigned int*)calloc(SCREEN_WIDTH*SCREEN_HEIGHT, sizeof(int));
  if(undoBuffer == NULL){
    if (!QMessageBox::warning(this, "CloverPaint", "Sorry, no available memory...\n","OK")){
      clover->close();
      return;
    }  
  }
  for(int i=0; i<SCREEN_WIDTH*SCREEN_HEIGHT; i++){
    *(undoBuffer+i) = 0xffffffff;
  }


  //Redoバッファ件 描画済チェック用バッファ
  drawChecker = redoBuffer = (unsigned int*)calloc(SCREEN_WIDTH*SCREEN_HEIGHT, sizeof(int));
  if(drawChecker == NULL){
    if (!QMessageBox::warning(this, "CloverPaint", "Sorry, no available memory...\n","OK")){
      clover->close();
      return;
    }  
  }

  for(int i=0; i<SCREEN_WIDTH*SCREEN_HEIGHT; i++){
    *(drawChecker+i) = 0x00ffffff;
  }


  toolbarY = TOOLBAR_Y1;
  huebarY = HUEBAR_Y1;

  QImage *tmpImg = new QImage(cloverimage);
  cloverImg = new QImage(tmpImg->convertDepth(32));
  delete tmpImg;

  tmpImg = new QImage(mode_normal);
  mode_normalImg = new QImage(tmpImg->convertDepth(32));
  delete tmpImg;

  tmpImg = new QImage(mode_reverse);
  mode_reverseImg = new QImage(tmpImg->convertDepth(32));
  delete tmpImg;

  tmpImg = new QImage(pensize_normal);
  pensize_normalImg = new QImage(tmpImg->convertDepth(32));
  delete tmpImg;

  tmpImg = new QImage(pensize_reverse);
  pensize_reverseImg = new QImage(tmpImg->convertDepth(32));
  delete tmpImg;

  tmpImg = new QImage(mesh_normal);
  mesh_normalImg = new QImage(tmpImg->convertDepth(32));
  delete tmpImg;
  tmpImg = new QImage(mesh_reverse);
  mesh_reverseImg = new QImage(tmpImg->convertDepth(32));
  delete tmpImg;

  tmpImg = new QImage(palette_normal);
  palette_normalImg = new QImage(tmpImg->convertDepth(32));
  delete tmpImg;

  tmpImg = new QImage(palette_reverse);
  palette_reverseImg = new QImage(tmpImg->convertDepth(32));
  delete tmpImg;

  tmpImg = new QImage(brushsize11);
  brush11 = new QImage(tmpImg->convertDepth(32));
  delete tmpImg;

  tmpImg = new QImage(brushsize17);
  brush17 = new QImage(tmpImg->convertDepth(32));
  delete tmpImg;

  tmpImg = new QImage(brushsize25);
  brush25 = new QImage(tmpImg->convertDepth(32));
  delete tmpImg;

  tmpImg = new QImage(brushsize33);
  brush33 = new QImage(tmpImg->convertDepth(32));
  delete tmpImg;

  tmpImg = new QImage(brushsize65);
  brush65 = new QImage(tmpImg->convertDepth(32));
  delete tmpImg;

  tmpImg = new QImage(brushsize99);
  brush99 = new QImage(tmpImg->convertDepth(32));
  delete tmpImg;


  colorPalette[0] = 0xffffffff;
  colorPalette[1] = 0xffffffff;
  colorPalette[2] = 0xffffffff;
  colorPalette[3] = 0xffffffff;
  colorPalette[4] = 0xffffffff;
  colorPalette[5] = 0xffffffff;
  colorPalette[6] = 0xffffffff;
  colorPalette[7] = 0xffffffff;
  colorPalette[8] = 0xffffffff;
  colorPalette[9] = 0xffffffff;


  for(int i=0; i<3; i++){
    for(int j=0; j<256; j++){
      colorMatrix[i][j] = j;
    }
  }
  useColorMatrix = false;

  //設定の読み込み
  if(QFile::exists("/home/zaurus/Settings/ZColorAdjust.conf")){
    QFile ff( "/home/zaurus/Settings/ZColorAdjust.conf" );

    useColorMatrix = true;

    ff.open( IO_ReadOnly );
    QDataStream ss( &ff );
    for(int i=0; i<3; i++){
      for(int j=0; j<256; j++){
	ss >> (Q_INT8 &)(colorMatrix[i][j]);
      }
    }
    ff.close();
  }





  if(QApplication::desktop()->width() == 640){
    windowDirection = YOKO;
    for(int y=0; y<640; y++){
      for(int x=0; x<480; x++){
	bgImage->setPixel(x,y, 0xff000000);
      }
    }
    for(int y=0; y<40; y++){
      for(int x=0; x<29; x++){
	bgImage->setPixel(305+x,260+y, cloverImg->pixel(x,y));
      }
    }

    QPixmap canvasPix = QPixmap();
    canvasPix.convertFromImage(*(bgImage),0);
    QPainter p;
    p.begin(&canvasPix );
    p.setPen( white );
    p.setFont(QFont("lcfont", 20));
    p.drawText(QRect(200,180, 320, 24),Qt::AlignLeft , "CloverPaint:");
    p.drawText(QRect(200,210, 320, 24),Qt::AlignLeft , "Please change to ViewStyle.");
    p.end();  
    setBackgroundPixmap(canvasPix);

  }
  else{
    windowDirection = TATE;
  }

  toolbar = new MyToolBar(this, "toolbar", false);
  if(windowDirection == TATE)
    toolbar->show();
  else
    toolbar->hide();
  huebar = new Hue(this, "hue", false);
  if(windowDirection == TATE)
    huebar->show();
  else
    huebar->hide();

  pensizePanel = new PenSize(this, "pensize", false);
  pensizePanel->hide();

  meshPanel = new Mesh(this, "mesh", false);
  meshPanel->hide();

  palettePanel = new Palette(this, "palette", false);
  palettePanel->hide();

  scalePanel = new Scale(this, "scale", false);
  scalePanel->hide();

  toolPanel = new Tool(this, "tool", false);
  toolPanel->hide();

  lupePanel = new Lupe(this, "lupe", false);
  lupePanel->hide();

  layerPanel = new Layer(this, "layer", false);
  layerPanel->hide();

  layerModePanel = new LayerMode(this, "layermode", false);
  layerModePanel->hide();

  drawModePanel = new DrawMode(this, "drawmode", false);
  drawModePanel->hide();

  fileMenuPanel = new FileMenu(this, "filemenu", false);
  fileMenuPanel->hide();

  selectPanel = new Select(this, "select", false);
  selectPanel->hide();



  int repaint_left;
  int repaint_top;

  int repaint_width;
  int repaint_height;

  //設定の読み込み
  if(QFile::exists("/home/zaurus/Settings/CloverPaint.conf")){
    QFile f( "/home/zaurus/Settings/CloverPaint.conf" );
    f.open( IO_ReadOnly );
    QDataStream s( &f );
    Q_INT32 version;
    s >> version;
    if(version == 104){
      s >> (Q_INT8 &)previousLayerMode; //直前のレイヤモード
      s >> previousDirectory; //前回選択したディレクトリ
      s >> lastScale; //前回選択した倍率

      QPoint p;
      Q_INT8 sticky;
      Q_INT8 display;

      s >> p;
      toolbar->move(p);

      s >> p;
      huebar->move(p);

      s >> p;
      s >> (Q_INT8 &)sticky;
      s >> (Q_INT8 &)display;
      pensizePanel->move(p);
      if(sticky == 1){
	pensizePanel->stickyFlag = true;
      }
      if(display == 1 && windowDirection == TATE){
	pensizePanel->display = true;
	if(windowDirection == TATE)
	  pensizePanel->show();
      }
      s >> p;
      s >> (Q_INT8 &)sticky;
      s >> (Q_INT8 &)display;
      meshPanel->move(p);
      if(sticky == 1){
	meshPanel->stickyFlag = true;
      }
      if(display == 1){
	meshPanel->display = true;
	if(windowDirection == TATE)
	  meshPanel->show();
      }

      s >> p;
      s >> (Q_INT8 &)sticky;
      s >> (Q_INT8 &)display;
      scalePanel->move(p);
      if(sticky == 1){
	scalePanel->stickyFlag = true;
      }
      if(display == 1){
	scalePanel->display = true;
	if(windowDirection == TATE)
	  scalePanel->show();
      }

      s >> p;
      s >> (Q_INT8 &)sticky;
      s >> (Q_INT8 &)display;
      toolPanel->move(p);
      if(sticky == 1){
	toolPanel->stickyFlag = true;
      }
      if(display == 1){
	toolPanel->display = true;
	if(windowDirection == TATE)
	  toolPanel->show();
      }

      s >> p;
      s >> (Q_INT8 &)sticky;
      s >> (Q_INT8 &)display;
      selectPanel->move(p);
      if(sticky == 1){
	selectPanel->stickyFlag = true;
      }
      if(display == 1){
	selectPanel->display = true;
	if(windowDirection == TATE)
	  selectPanel->show();
      }

      s >> p;
      lupePanel->move(p);

      s >> p;
      s >> (Q_INT8 &)sticky;
      s >> (Q_INT8 &)display;
      layerPanel->move(p);
      if(sticky == 1){
	layerPanel->stickyFlag = true;
      }
      if(display == 1){
	layerPanel->display = true;
	if(windowDirection == TATE)
	  layerPanel->show();
      }


      s >> p;
      s >> (Q_INT8 &)sticky;
      s >> (Q_INT8 &)display;
      palettePanel->move(p);
      if(sticky == 1){
	palettePanel->stickyFlag = true;
      }
      if(display == 1){
	palettePanel->display = true;
	if(windowDirection == TATE)
	  palettePanel->show();
      }


      Q_INT32 tmpCol;
      for(int t=0;t<10;t++){
	s >> (Q_INT32 &)tmpCol;
	colorPalette[t] = tmpCol;
      }

      Q_INT8 tmpColorMatrix;
      s >> (Q_INT8 &)tmpColorMatrix;
      if(tmpColorMatrix == 1){
	useColorMatrix = true;
      }
      else{
	useColorMatrix = false;
      }


    }
  }

  toolbarY = TOOLBAR_Y1;
  meshPanelY = MESHPANEL_Y1;
  palettePanelY = PALETTEPANEL_Y1;
  pensizePanelY = PENSIZEPANEL_Y1;
  scalePanelY = SCALEPANEL_Y1;
  toolPanelY = TOOLPANEL_Y1;
  huebarY = HUEBAR_Y1;
  layerPanelY = 640 - 80 - (44+20+29*layernum);
  fileMenuPanelY = 640 - 31 - 20 - fileMenuPanel->height();
  selectPanelY = 640-31-20-selectPanel->height();
  drawModePanelY = 640-23-20-drawModePanel->height();

  if(huebar->y() == 0){
    toolbarY = TOOLBAR_Y2;
    huebarY = HUEBAR_Y2;
    pensizePanelY = PENSIZEPANEL_Y2;
    meshPanelY = MESHPANEL_Y2;
    palettePanelY = PALETTEPANEL_Y2;
    scalePanelY = SCALEPANEL_Y2;
    toolPanelY = SCALEPANEL_Y2;
    layerPanelY = LAYERPANEL_Y2;
    fileMenuPanelY = FILEMENUPANEL_Y2;
    selectPanelY = SELECTPANEL_Y2;
    drawModePanelY = DRAWMODEPANEL_Y2;
  }


  repaint_left = 477;
  repaint_top = 0;
  repaint_width = 3;
  repaint_height = 20;
  toolbar->repaintColor(repaint_left, repaint_top, repaint_width, repaint_height, false);

  //  clover->activateFlag = true;

  /*
  int c = 0;
  int c2 = 0;
  int c3 = 0;
  int c4 = 0;

    for(int r=0;r<255;r++){
    for(int g=0;g<255;g++){
    for(int b=0;b<255;b++){

    pencolor = 0xff000000 | ((r << 16)&0x00ff0000) | ((g << 8)&0x0000ff00) | ((b)&0x000000ff);
    RGB2HSB();
    HSB2RGBd();
    int r2 = (pencolor >> 16)&0x000000ff;
    int g2 = (pencolor >> 8)&0x000000ff;
    int b2 = (pencolor >> 0)&0x000000ff;

    if(r!=r2 || g!=g2 || b!=b2){
      c++;
      if(abs(r-r2) == 2 || abs(g-g2) == 2 || abs(b-b2) == 2){
	c2++;
      }
      if(abs(r-r2) == 3 || abs(g-g2) == 3 || abs(b-b2) == 3){
	c3++;
      }
      if(abs(r-r2) > 3 || abs(g-g2) > 3 || abs(b-b2) > 3){
	c4++;
//	fprintf(stderr,"%d:%d:%d,%d:%d:%d\n",r,g,b,r2,g2,b2);
      }
      
    }
    }

    }
    }
    fprintf(stderr, "%d,%d,%d,%d\n",c,c2,c3,c4);
  */





    toolbar->changeSBArea();
    toolbar->changeColorArea();

    //ツールバー画像の表示
    //    QPixmap toolbarPix = QPixmap();
    //    toolbarPix.convertFromImage(toolbar->toolbarImg, 0);
    //    toolbar->setBackgroundPixmap(toolbarPix);
    toolbar->repaint_left = SBAREA_X-2;
    toolbar->repaint_top = SBAREA_Y-2;
    toolbar->repaint_width = SBAREA_WIDTH+4;
    toolbar->repaint_height = SBAREA_HEIGHT+4;
    QRegion r(toolbar->repaint_left,toolbar->repaint_top,toolbar->repaint_width,toolbar->repaint_height,QRegion::Rectangle);
    toolbar->repaint(r, false);

    toolbar->repaint_left = COLORAREA_X;
    toolbar->repaint_top = COLORAREA_Y;
    toolbar->repaint_width = COLORAREA_WIDTH;
    toolbar->repaint_height = COLORAREA_HEIGHT;
    r =QRegion(toolbar->repaint_left,toolbar->repaint_top,toolbar->repaint_width,toolbar->repaint_height,QRegion::Rectangle);
    toolbar->repaint(r, false);


    toolbar->repaint_left = COLORAREA3_X;
    toolbar->repaint_top = COLORAREA3_Y;
    toolbar->repaint_width = COLORAREA3_WIDTH;
    toolbar->repaint_height = COLORAREA3_HEIGHT;
    r =QRegion(toolbar->repaint_left,toolbar->repaint_top,toolbar->repaint_width,toolbar->repaint_height,QRegion::Rectangle);
    toolbar->repaint(r, false);
    
    //    QPixmap huePix = QPixmap();
    //    huePix.convertFromImage(huebar->hueImg, 0);
    //    huebar->setBackgroundPixmap(huePix);
    
    repaint_left = 479-(int)(479*fgHu/COLORDIV)-1;
    if(repaint_left < 0){
      repaint_left = 0;
    }
    else if(repaint_left > 477){
      repaint_left = 477;
    }
    repaint_top = 0;
    repaint_width = 3;
    repaint_height = 20;
    toolbar->repaintColor(repaint_left, repaint_top, repaint_width, repaint_height, false);


}

Canvas::~Canvas()
{

  QFile f( "/home/zaurus/Settings/CloverPaint.conf" );
  f.open( IO_WriteOnly );
  QDataStream s( &f );
  s << (Q_INT32)104; //設定ファイルバージョン
  s << (Q_INT8)previousLayerMode; //直前のレイヤモード
  s << (QString)previousDirectory; //前回選択したディレクトリ
  s << (Q_INT32)lastScale; //前回選択した倍率

  s << (QPoint)toolbar->pos();
  s << (QPoint)huebar->pos();

  s << (QPoint)pensizePanel->pos();
  if(pensizePanel->stickyFlag)
    s << (Q_INT8)1;
  else
    s << (Q_INT8)0;
  if(pensizePanel->display)
    s << (Q_INT8)1;
  else
    s << (Q_INT8)0;

  s << (QPoint)meshPanel->pos();
  if(meshPanel->stickyFlag)
    s << (Q_INT8)1;
  else
    s << (Q_INT8)0;
  if(meshPanel->display)
    s << (Q_INT8)1;
  else
    s << (Q_INT8)0;

  s << (QPoint)scalePanel->pos();
  if(scalePanel->stickyFlag)
    s << (Q_INT8)1;
  else
    s << (Q_INT8)0;
  if(scalePanel->display)
    s << (Q_INT8)1;
  else
    s << (Q_INT8)0;

  s << (QPoint)toolPanel->pos();
  if(toolPanel->stickyFlag)
    s << (Q_INT8)1;
  else
    s << (Q_INT8)0;
  if(toolPanel->display)
    s << (Q_INT8)1;
  else
    s << (Q_INT8)0;


  s << (QPoint)selectPanel->pos();
  if(selectPanel->stickyFlag)
    s << (Q_INT8)1;
  else
    s << (Q_INT8)0;
  if(selectPanel->display)
    s << (Q_INT8)1;
  else
    s << (Q_INT8)0;

  s << (QPoint)lupePanel->pos();

  s << (QPoint)layerPanel->pos();
  if(layerPanel->stickyFlag)
    s << (Q_INT8)1;
  else
    s << (Q_INT8)0;
  if(layerPanel->display)
    s << (Q_INT8)1;
  else
    s << (Q_INT8)0;

  s << (QPoint)palettePanel->pos();
  if(palettePanel->stickyFlag)
    s << (Q_INT8)1;
  else
    s << (Q_INT8)0;
  if(palettePanel->display)
    s << (Q_INT8)1;
  else
    s << (Q_INT8)0;


  for(int t=0;t<10;t++){
    s << (Q_INT32)colorPalette[t];
  }

  Q_INT8 tmpColorMatrix;
  if(useColorMatrix){
    tmpColorMatrix = 1;
  }
  else{
    tmpColorMatrix = 0;
  }
  s << (Q_INT8)tmpColorMatrix;


  f.close();
  
  free(drawChecker);
  free(undoBuffer);


  delete toolbar;
  delete huebar;
  delete pensizePanel;
  delete meshPanel;
  delete palettePanel;
  delete scalePanel;
  delete toolPanel;
  delete lupePanel;
  delete layerPanel;
  delete layerModePanel;
  delete drawModePanel;
  delete fileMenuPanel;
  delete selectPanel;

  delete bgImage;
  delete cloverImg;
  delete mode_normalImg;
  delete mode_reverseImg;
  delete pensize_normalImg;
  delete mesh_normalImg;
  delete pensize_reverseImg;
  delete mesh_reverseImg;

  delete brushImage;
  delete brush11;
  delete brush17;
  delete brush25;
  delete brush33;
  delete brush65;
  delete brush99;

  delete[] layer;
}


void Canvas::CalcHue(unsigned int *tmpR1, unsigned int *tmpG1, unsigned int *tmpB1, unsigned int tmpR2, unsigned int tmpG2, unsigned int tmpB2)
{
  int r,g,b;
  int max;// r,g,bの最大値
  int min;// r,g,bの最小値
  int d;
  int tmpHu, tmpSa;
  int tmpBr;
  unsigned int phase, ofs;

  //b - rなどの演算のためにあらかじめunsignedを外す必要がある.
  r = (int)tmpR2;
  g = (int)tmpG2;
  b = (int)tmpB2;

  // 最大の色、最小の色を求める
  max = ( r > g ) ? r : g;
  if( b > max ) max = b;
  min = ( r < g ) ? r : g;
  if( b < min ) min = b;
  d = max - min;
  if(d == 0 || max == 0) {
    tmpHu = 0;
  }
  else {
    if(r == max) {
      tmpHu = ((-b + g)<<8)/(6*d);
    }
    else if(g == max) {
      tmpHu = 85 + ((-r + b)<<8)/(6*d);
    }
    else {
      tmpHu = 170 + ((- g + r)<<8)/(6*d);
    }

    if( tmpHu < 0) {
      tmpHu += 255;
    }
  }
  //  printf("tmpHu:%f\n",tmpHu);

  // 下の彩度、明度を求める
  r = (int)*tmpR1;
  g = (int)*tmpG1;
  b = (int)*tmpB1;

  // 最大の色、最小の色を求める
  max = ( r > g ) ? r : g;
  if( b > max ) max = b;
  min = ( r < g ) ? r : g;
  if( b < min ) min = b;
  d = max - min;
  // 明度を求める
  tmpBr = max;
  // 彩度を求める(黒なら彩度なし)
  if(max != 0) {
    tmpSa = (d<<8)/max;
  }
  else {
    tmpSa = 0;// 黒
  }

  //RGBへ変換する
  //  printf("H:%d,S:%d,B:%d\n,",tmpHu,tmpSa,tmpBr);

  // グレイのとき

  if(tmpSa == 0) {
    *tmpR1 = *tmpG1 = *tmpB1 = (unsigned int)tmpBr;
    return;
  }
  phase = (tmpHu*360*6)>>8;
  ofs = phase % 360;

  switch((tmpHu*6)>>8) {
  case 0:
    *tmpR1 = (unsigned int)tmpBr;
    *tmpG1 = (tmpBr * ( 255 - tmpSa + (tmpSa * ofs )/360))>>8;
    *tmpB1 = tmpBr - ((tmpBr * tmpSa )>>8);
    break;
  case 1:
    *tmpR1 = tmpBr - (((tmpBr * tmpSa * ofs) / 360)>>8);
    *tmpG1 = (unsigned int)tmpBr;
    *tmpB1 = tmpBr - ((tmpBr * tmpSa )>>8);
    break;
  case 2:
    *tmpR1 = tmpBr - ((tmpBr * tmpSa )>>8);
    *tmpG1 = (unsigned int)tmpBr;
    *tmpB1 = (tmpBr * ( 255 - tmpSa + (tmpSa * ofs )/360))>>8;
    break;
  case 3:
    *tmpR1 = tmpBr - ((tmpBr * tmpSa )>>8);
    *tmpG1 = tmpBr - (((tmpBr * tmpSa * ofs) / 360)>>8);
    *tmpB1 = (unsigned int)tmpBr;
    break;
  case 4:
    *tmpR1 = (tmpBr * ( 255 - tmpSa + (tmpSa * ofs )/360))>>8;
    *tmpG1 = tmpBr - ((tmpBr * tmpSa )>>8);
    *tmpB1 = (unsigned int)tmpBr;
    break;
  case 5:
    *tmpR1 = (unsigned int)tmpBr;
    *tmpG1 = tmpBr - ((tmpBr * tmpSa )>>8);
    *tmpB1 = tmpBr - (((tmpBr * tmpSa * ofs) / 360)>>8);
    break;
  }


}


void Canvas::CalcColor(unsigned int *tmpR1, unsigned int *tmpG1, unsigned int *tmpB1, unsigned int tmpR2, unsigned int tmpG2, unsigned int tmpB2)
{
  int r,g,b;
  int rr=0;
  int gg=0;
  int bb=0;
  int max;// r,g,bの最大値
  int min;// r,g,bの最小値
  int d;
  int tmpHu, tmpSa, tmpSa2;
  int tmpL, tmpL2;
  int tmpBr;
  unsigned int phase, ofs;

  // 上の色相と下の輝度が保存

  // 上の色相を求める

  //b - rなどの演算のためにあらかじめunsignedを外す必要がある.
  r = (int)tmpR2;
  g = (int)tmpG2;
  b = (int)tmpB2;

  // 最大の色、最小の色を求める
  max = ( r > g ) ? r : g;
  if( b > max ) max = b;
  min = ( r < g ) ? r : g;
  if( b < min ) min = b;
  d = max - min;
  tmpBr = max; 


  // 彩度を求める(黒なら彩度なし)
  if(max != 0) {
    tmpSa = (d<<8)/max;
  }
  else {
    tmpSa = 0;// 黒
  }

  /*  // グレイのとき
  if(tmpSa == 0) {
    *tmpR1 = *tmpG1 = *tmpB1 = (unsigned int)tmpBr;
    return;
  }
  */

  if(d == 0 || max == 0) {
    tmpHu = 0;
  }
  else {
    if(r == max) {
      tmpHu = ((-b + g)<<8)/(6*d);
    }
    else if(g == max) {
      tmpHu = 85 + ((-r + b)<<8)/(6*d);
    }
    else {
      tmpHu = 170 + ((- g + r)<<8)/(6*d);
    }

    if( tmpHu < 0) {
      tmpHu += 255;
    }
  }


  // 下の輝度を求める

  // rVal + gVal + bVal = 256
  int rVal = 77;
  int gVal = 151;
  int bVal = 28;

  // tmpL = (0.30 * r + 0.59 * g + 0.11* b);
  // 本当は256で割る必要があるが、方程式で利用するだけなのでそのまま。
   tmpL = rVal * (int)*tmpR1 + gVal * (int)*tmpG1 + bVal * (int)*tmpB1;

  //---------------------------------
  // 解くべき方程式
  // 
  // 求めたい変数: rr, gg, bb
  // 定数: tmpL, d
  // 方程式:
  // tmpL = 77 * rr + 151 * gg + 28 * bb
  // (r, g, bのうち最大の色がrr, gg, bbでも最大。)
  // (r, g, bのうち最小の色がrr, gg, bbでも最小。)
  // max(rr, gg, bb)
  // d = max(rr, gg, bb) - min(rr, gg, bb)
  // 
  // 結果色の色相は tmpHu
  // 結果色の彩度は tmpSa = (d<<8)/max(rr, gg, bb);
  // 結果色の明度は tmpBr = max(rr, gg, bb)


  //RGBへ変換する
  phase = (tmpHu*360*6)>>8;
  ofs = phase % 360;

  // rが最大のとき
  // tmpSa = (d<<8)/rr
  // tmpBr = rr
  if(r == max){
    switch((tmpHu*6)>>8) {
    case 0:
      // rr = rr;
      // 77rr = tmpL - 151gg - bValbb
      // 77rr = tmpL - 151(rr - d + d * ofs /360) - 28(rr-d)
      // (256)rr = tmpL + 59*(d - d * ofs /360) + 11*d
      rr = (tmpL + gVal*(d- d*ofs/360) + bVal*d)>>8;
      gg = rr - d + d*ofs/360;
      bb = rr - d;
      break;
    case 1:
      rr = 0;
      gg = 0;
      bb = 0;
      break;
    case 2:
      rr = 0;
      gg = 0;
      bb = 0;
      break;
    case 3:
      rr = 0;
      gg = 0;
      bb = 0;
      break;
    case 4:
      // d =  ((d) * ofs )/360) ?????
      rr = (tmpL + gVal*d)>>8;
      gg = rr - d;
      bb = rr;
      break;
    case 5:
      rr = (tmpL + gVal*d + bVal*(d*ofs/360))>>8;
      gg = rr - d;
      bb = rr - d * ofs / 360;
      break;
    }
  }
  // gが最大のとき
  // tmpSa = (d<<8)/gg
  // tmpBr = gg
  // 151gg = tmpL - 77rr - 28bb
  else if(g == max){
    switch((tmpHu*6)>>8) {
    case 0:
      // gg = gg - d + d * ofs /360; ???
      gg = (tmpL + bVal*d)>>8;
      rr = gg;
      bb = gg - d;
      break;
    case 1:
      gg = (tmpL + rVal*d*ofs/360 + bVal*d)>>8;
      rr = gg - d*ofs/360;
      bb = gg - d;
      break;
    case 2:
      gg = (tmpL + rVal*d + bVal*(d - d*ofs/360))>>8;
      rr = gg - d;
      bb = gg - d + d*ofs/360;
      break;
    case 3:
      rr = 0;
      gg = 0;
      bb = 0;
      break;
    case 4:
      rr = 0;
      gg = 0;
      bb = 0;
      break;
    case 5:
      rr = 0;
      gg = 0;
      bb = 0;
      break;
    }
  }
  // bが最大のとき
  // tmpSa = (d<<8)/bb
  // tmpBr = bb
  // 28bb = tmpL - 77rr - 151gg
  else if(b == max){
    switch((tmpHu*6)>>8) {
    case 0:
      rr = 0;
      gg = 0;
      bb = 0;
      break;
    case 1:
      rr = 0;
      gg = 0;
      bb = 0;
      break;
    case 2:
      bb = (tmpL + rVal*d)>>8;
      rr = bb - d;
      gg = bb;
      // bb = bb - d + d * ofs / 360; ???
      break;
    case 3:
      bb = (tmpL + rVal*d + gVal*d*ofs/360)>>8;
      rr = bb - d;
      gg = bb - d*ofs/360;
      break;
    case 4:
      bb = (tmpL + rVal*(d - d*ofs/360) + gVal*d)>>8;
      rr = bb - d + d*ofs/360;
      gg = bb - d;
      break;
    case 5:
      rr = 0;
      gg = 0;
      bb = 0;
      break;
    }
  }
  
  if(rr > 255){
    *tmpR1 = 255;
  }
  else if(rr < 0){
    *tmpR1 = 0;
  }
  else{
    *tmpR1 = rr;
  }
  if(gg > 255){
    *tmpG1 = 255;
  }
  else if(gg < 0){
    *tmpG1 = 0;
  }
  else{
    *tmpG1 = gg;
  }
  if(bb > 255){
    *tmpB1 = 255;
  }
  else if(bb < 0){
    *tmpB1 = 0;
  }
  else{
    *tmpB1 = bb;
  }

}


void Canvas::CalcSaturation(unsigned int *tmpR1, unsigned int *tmpG1, unsigned int *tmpB1, unsigned int tmpR2, unsigned int tmpG2, unsigned int tmpB2)
{
  int r,g,b;
  int max;// r,g,bの最大値
  int min;// r,g,bの最小値
  int d;
  int tmpHu, tmpSa, tmpSa2;
  int tmpBr;
  unsigned int phase, ofs;

  // 上の彩度を求める
  //b - rなどの演算のためにあらかじめunsignedを外す必要がある.
  r = (int)tmpR2;
  g = (int)tmpG2;
  b = (int)tmpB2;

  // 最大の色、最小の色を求める
  max = ( r > g ) ? r : g;
  if( b > max ) max = b;
  min = ( r < g ) ? r : g;
  if( b < min ) min = b;
  d = max - min;
  // 彩度を求める(黒なら彩度なし)
  if(max != 0) {
    tmpSa = (d<<8)/max;
  }
  else {
    tmpSa = 0;// 黒
  }

  // 下の色相、明度を求める
  r = (int)*tmpR1;
  g = (int)*tmpG1;
  b = (int)*tmpB1;

  // 最大の色、最小の色を求める
  max = ( r > g ) ? r : g;
  if( b > max ) max = b;
  min = ( r < g ) ? r : g;
  if( b < min ) min = b;
  d = max - min;
  // 明度を求める
  tmpBr = max;

  //もし下の彩度が0なら変化なし
  if(max != 0) {
    tmpSa2 = (d<<8)/max;
  }
  else {
    tmpSa2 = 0;// 黒
  }
  if(tmpSa2 == 0){
    return;
  }


  if(d == 0 || max == 0) {
    tmpHu = 0;
  }
  else {
    if(r == max) {
      tmpHu = ((-b + g)<<8)/(6*d);
    }
    else if(g == max) {
      tmpHu = 85 + ((-r + b)<<8)/(6*d);
    }
    else {
      tmpHu = 170 + ((- g + r)<<8)/(6*d);
    }

    if( tmpHu < 0) {
      tmpHu += 255;
    }
  }

  //RGBへ変換する
  //  printf("H:%d,S:%d,B:%d\n,",tmpHu,tmpSa,tmpBr);

  // グレイのとき

  if(tmpSa == 0) {
    *tmpR1 = *tmpG1 = *tmpB1 = (unsigned int)tmpBr;
    return;
  }
  phase = (tmpHu*360*6)>>8;
  ofs = phase % 360;

  switch((tmpHu*6)>>8) {
  case 0:
    *tmpR1 = (unsigned int)tmpBr;
    *tmpG1 = (tmpBr * ( 255 - tmpSa + (tmpSa * ofs )/360))>>8;
    *tmpB1 = tmpBr - ((tmpBr * tmpSa )>>8);
    break;
  case 1:
    *tmpR1 = tmpBr - (((tmpBr * tmpSa * ofs) / 360)>>8);
    *tmpG1 = (unsigned int)tmpBr;
    *tmpB1 = tmpBr - ((tmpBr * tmpSa )>>8);
    break;
  case 2:
    *tmpR1 = tmpBr - ((tmpBr * tmpSa )>>8);
    *tmpG1 = (unsigned int)tmpBr;
    *tmpB1 = (tmpBr * ( 255 - tmpSa + (tmpSa * ofs )/360))>>8;
    break;
  case 3:
    *tmpR1 = tmpBr - ((tmpBr * tmpSa )>>8);
    *tmpG1 = tmpBr - (((tmpBr * tmpSa * ofs) / 360)>>8);
    *tmpB1 = (unsigned int)tmpBr;
    break;
  case 4:
    *tmpR1 = (tmpBr * ( 255 - tmpSa + (tmpSa * ofs )/360))>>8;
    *tmpG1 = tmpBr - ((tmpBr * tmpSa )>>8);
    *tmpB1 = (unsigned int)tmpBr;
    break;
  case 5:
    *tmpR1 = (unsigned int)tmpBr;
    *tmpG1 = tmpBr - ((tmpBr * tmpSa )>>8);
    *tmpB1 = tmpBr - (((tmpBr * tmpSa * ofs) / 360)>>8);
    break;
  }


}



void Canvas::CalcBrightness(unsigned int *tmpR1, unsigned int *tmpG1, unsigned int *tmpB1, unsigned int tmpR2, unsigned int tmpG2, unsigned int tmpB2)
{
  int r,g,b;
  int max;// r,g,bの最大値
  int min;// r,g,bの最小値
  int d;
  int tmpHu, tmpSa, tmpSa2;
  int tmpBr;
  unsigned int phase, ofs;

  // 上の明度を求める

  // ここでは輝度を用いるので、tmpBrはr,g,bの重み付き総和

  //b - rなどの演算のためにあらかじめunsignedを外す必要がある.
  r = (int)tmpR2;
  g = (int)tmpG2;
  b = (int)tmpB2;

  //  tmpBr = (0.30 * r + 0.59 * g + 0.11* b)*3;
  tmpBr = (230 * r + 453 * g + 85 * b)>>8;


  //下の色相，彩度を求める
  r = (int)*tmpR1;
  g = (int)*tmpG1;
  b = (int)*tmpB1;
  // 最大の色、最小の色を求める
  max = ( r > g ) ? r : g;
  if( b > max ) max = b;
  min = ( r < g ) ? r : g;
  if( b < min ) min = b;
  d = max - min;
  
  if(max != 0) {
    tmpSa = (d<<8)/max;
  }
  else {
    tmpSa = 0;// 黒
  }

  if(tmpBr > 255){
    // 明度だけでは輝度を実現できなかった場合、彩度で補う
    tmpSa2 = 255 - ((tmpBr - 255)>>1);
    if(tmpSa2 < tmpSa){
      tmpSa = tmpSa2;
    }
    tmpBr = 255;
  }
  


  if(d == 0 || max == 0) {
    tmpHu = 0;
  }
  else {
    if(r == max) {
      tmpHu = ((-b + g)<<8)/(6*d);
    }
    else if(g == max) {
      tmpHu = 85 + ((-r + b)<<8)/(6*d);
    }
    else {
      tmpHu = 170 + ((- g + r)<<8)/(6*d);
    }

    if( tmpHu < 0) {
      tmpHu += 255;
    }
  }

  // 上のレイヤの明度の補数と下のレイヤの彩度の小さいほうを彩度とする
  //  if(255-tmpBr < tmpSa){
  //    tmpSa = 255 - tmpBr;
  //  }


  //RGBへ変換する
  //  printf("H:%d,S:%d,B:%d\n,",tmpHu,tmpSa,tmpBr);

  // グレイのとき

  if(tmpSa == 0) {
    *tmpR1 = *tmpG1 = *tmpB1 = (unsigned int)tmpBr;
    return;
  }
  phase = (tmpHu*360*6)>>8;
  ofs = phase % 360;

  switch((tmpHu*6)>>8) {
  case 0:
    *tmpR1 = (unsigned int)tmpBr;
    *tmpG1 = (tmpBr * ( 255 - tmpSa + (tmpSa * ofs )/360))>>8;
    *tmpB1 = tmpBr - ((tmpBr * tmpSa )>>8);
    break;
  case 1:
    *tmpR1 = tmpBr - (((tmpBr * tmpSa * ofs) / 360)>>8);
    *tmpG1 = (unsigned int)tmpBr;
    *tmpB1 = tmpBr - ((tmpBr * tmpSa )>>8);
    break;
  case 2:
    *tmpR1 = tmpBr - ((tmpBr * tmpSa )>>8);
    *tmpG1 = (unsigned int)tmpBr;
    *tmpB1 = (tmpBr * ( 255 - tmpSa + (tmpSa * ofs )/360))>>8;
    break;
  case 3:
    *tmpR1 = tmpBr - ((tmpBr * tmpSa )>>8);
    *tmpG1 = tmpBr - (((tmpBr * tmpSa * ofs) / 360)>>8);
    *tmpB1 = (unsigned int)tmpBr;
    break;
  case 4:
    *tmpR1 = (tmpBr * ( 255 - tmpSa + (tmpSa * ofs )/360))>>8;
    *tmpG1 = tmpBr - ((tmpBr * tmpSa )>>8);
    *tmpB1 = (unsigned int)tmpBr;
    break;
  case 5:
    *tmpR1 = (unsigned int)tmpBr;
    *tmpG1 = tmpBr - ((tmpBr * tmpSa )>>8);
    *tmpB1 = tmpBr - (((tmpBr * tmpSa * ofs) / 360)>>8);
    break;
  }

}


void Canvas::RGB2HSB()
{
  double r,g,b;
  double max;// r,g,bの最大値
  double min;// r,g,bの最小値
  double dr, dg, db;
  double d;
  double tmpHu, tmpSa, tmpBr;

  r = (double)((pencolor >> 16) & 0x0000ff)/((double)255);
  g = (double)((pencolor >> 8) & 0x0000ff)/((double)255);
  b = (double)((pencolor) & 0x0000ff)/((double)255);

  // 最大の色、最小の色を求める
  max = ( r > g ) ? r : g;
  if( b > max ) max = b;
  min = ( r < g ) ? r : g;
  if( b < min ) min = b;
  d = max - min;
  // 明度を求める
  tmpBr = max;
  // 彩度を求める(黒なら彩度なし)
  if(max != 0.0) {
    tmpSa = d / max;
  }
  else {
    tmpSa = 0.0;// 黒
  }
  // 色相を求める
  if(tmpSa == 0.0) {
    tmpHu = 0.0;// グレイのとき
  }
  else {
    dr = (max - r) / d;
    dg = (max - g) / d;
    db = (max - b) / d;
    if(r == max) {
      tmpHu = db - dg;
    }
    else if(g == max) {
      tmpHu = 2.0 + dr - db;
    }
    else {
      tmpHu = 4.0 + dg - dr;
    }
    tmpHu /= 6.0;
    if( tmpHu < 0.0) {
      tmpHu += 1.0;
    }
  }

  fgSa2 = tmpSa;
  fgBr2 = tmpBr;
  fgHu2 = tmpHu;

  fgSa = (unsigned int)(tmpSa*COLORDIV);
  fgBr = (unsigned int)(tmpBr*COLORDIV);
  fgHu = (unsigned int)(tmpHu*COLORDIV);


}



void Canvas::HSB2RGBd(){
  unsigned int uH, p, q, s, t;
  unsigned int fgR=0, fgG=0, fgB=0;
  double uF;

  if(fgSa == 0){
    fgR = floor(fgBr2*255.0 + 0.5);
    fgG = floor(fgBr2*255.0 + 0.5);
    fgB = floor(fgBr2*255.0 + 0.5);
  }
  else{
    uH = (int)floor(fgHu2*6.0);

    uF = fgHu2*6.0 - uH;

    p = floor( fgBr2*(1.0-fgSa2)*255.0 + 0.5);

    q = floor( fgBr2*(1.0-(fgSa2 * uF))*255.0 + 0.5);

    s = floor( fgBr2*255.0 + 0.5);

    t = floor( fgBr2*(1.0-(fgSa2 * (1.0 - uF)))*255.0 + 0.5);


    if(p>255)
      p=255;
    if(q>255)
      q=255;
    if(s>255)
      s=255;
    if(t>255)
      t=255;

    switch(uH){
    case 0:
      fgR = s&0xff;
      fgG = t&0xff;
      fgB = p&0xff;
      break;
    case 1:
      fgR = q&0xff;
      fgG = s&0xff;
      fgB = p&0xff;		
      break;
    case 2:
      fgR = p&0xff;
      fgG = s&0xff;
      fgB = t&0xff;		
      break;
    case 3:
      fgR = p&0xff;
      fgG = q&0xff;			
      fgB = s&0xff;		
      break;
    case 4:
      fgR = t&0xff;
      fgG = p&0xff;			
      fgB = s&0xff;
      break;
    default:
      fgR = s&0xff;
      fgG = p&0xff;			
      fgB = q&0xff;		
      break;
    }
  }

  pencolor = 0xff000000 | ((fgR << 16)&0x00ff0000) | ((fgG << 8)&0x0000ff00) | ((fgB)&0x000000ff);

}


void Canvas::HSB2RGB(){
  unsigned int uH, uF, p, q, s, t;
  unsigned int fgR=0, fgG=0, fgB=0;

  if(fgSa == 0){
    fgR = fgBr*255/COLORDIV;	
    fgG = fgBr*255/COLORDIV;	
    fgB = fgBr*255/COLORDIV;
  }
  else{
    uH = fgHu*360*6/COLORDIV;

    uF = uH % 360;

    p = fgBr*(COLORDIV-fgSa)*255/COLORDIV/COLORDIV;

    q = fgBr*(COLORDIV-(fgSa * uF)/360)*255/COLORDIV/COLORDIV;

    s = fgBr*255/COLORDIV;

    t = fgBr*(COLORDIV-(fgSa * (360 - uF))/360)*255/COLORDIV/COLORDIV;

    if(p>255)
      p=255;
    if(q>255)
      q=255;
    if(s>255)
      s=255;
    if(t>255)
      t=255;

    switch(uH / 360){
    case 0:
      fgR = s&0xff;
      fgG = t&0xff;
      fgB = p&0xff;
      break;
    case 1:
      fgR = q&0xff;
      fgG = s&0xff;
      fgB = p&0xff;		
      break;
    case 2:
      fgR = p&0xff;
      fgG = s&0xff;
      fgB = t&0xff;		
      break;
    case 3:
      fgR = p&0xff;
      fgG = q&0xff;			
      fgB = s&0xff;		
      break;
    case 4:
      fgR = t&0xff;
      fgG = p&0xff;			
      fgB = s&0xff;
      break;
    default:
      fgR = s&0xff;
      fgG = p&0xff;			
      fgB = q&0xff;		
      break;
    }
  }

  pencolor = 0xff000000 | ((fgR << 16)&0x00ff0000) | ((fgG << 8)&0x0000ff00) | ((fgB)&0x000000ff);

}


void Canvas::showToolbar1(){
  toolbar->show();
  huebar->show();
}


void Canvas::changeToolbarVisible(){
  if(toolbar->isVisible()){
    toolbar->hide();
    huebar->hide();
    pensizePanel->hide();
    meshPanel->hide();
    palettePanel->hide();
    scalePanel->hide();
    toolPanel->hide();
    selectPanel->hide();
    layerPanel->hide();
    layerModePanel->hide();
    drawModePanel->hide();
    fileMenuPanel->hide();
  }
  else{
    toolbar->show();
    huebar->show();

    if(pensizePanel->display){
      pensizePanel->show();    
    }

    if(meshPanel->display){
      meshPanel->show();    
    }

    if(palettePanel->display){
      palettePanel->show();    
    }

    if(scalePanel->display){
      scalePanel->show();    
    }

    if(toolPanel->display){
      toolPanel->show();    
    }

    if(selectPanel->display){
      selectPanel->show();    
    }

    if(layerPanel->display){
      layerPanel->show();    
    }

  }


}


unsigned int Canvas::calc(short command, unsigned int ope1, unsigned int ope2, int layeralpha){
  unsigned int tmpR1, tmpG1, tmpB1;
  int tmpR11, tmpG11, tmpB11;
  unsigned int tmpR2, tmpG2, tmpB2;
  int alpha2;
  unsigned int tmpOpe2;

  tmpR1 = (ope1 >> 16) & 0x000000ff;
  tmpG1 = (ope1 >> 8) & 0x000000ff;
  tmpB1 = (ope1) & 0x000000ff;
  tmpR2 = (ope2 >> 16) & 0x000000ff;
  tmpG2 = (ope2 >> 8) & 0x000000ff;
  tmpB2 = (ope2) & 0x000000ff;
  alpha2 = ((ope2>>24)&0x000000ff);
  if(alpha2 == 255){
    alpha2 = 256;
  }

  if(alpha2 == 0){
    return ope1;
  }

  switch(command){
  case NORMAL:

    tmpR1 = tmpR1 - (((tmpR1 - tmpR2) * alpha2 * layeralpha)>>16);
    tmpG1 = tmpG1 - (((tmpG1 - tmpG2) * alpha2 * layeralpha)>>16);
    tmpB1 = tmpB1 - (((tmpB1 - tmpB2) * alpha2 * layeralpha)>>16);
    return 0xff000000 | ((tmpR1<<16)&0x00ff0000) | ((tmpG1<<8)&0x0000ff00) | (tmpB1&0x000000ff);

  case MULTIPLE:
    //    return 0xff000000 | ((((tmpR1 * tmpR2) >> 8)<<16)&0x00ff0000) | ((((tmpG1 * tmpG2) >> 8)<<8)&0x0000ff00) | ((((tmpB1 * tmpB2) >> 8))&0x000000ff);

    if(ope2 == 0xffffffff){
      return ope1;
    }

    tmpR2 = (255 - (((255 - tmpR2) * alpha2 * layeralpha)>>16))&0x000000ff;
    tmpG2 = (255 - (((255 - tmpG2) * alpha2 * layeralpha)>>16))&0x000000ff;
    tmpB2 = (255 - (((255 - tmpB2) * alpha2 * layeralpha)>>16))&0x000000ff;

    return 0xff000000 | (((tmpR1 * tmpR2)<< 8)&0x00ff0000) | (((tmpG1 * tmpG2))&0x0000ff00) | (((tmpB1 * tmpB2) >> 8)&0x000000ff);

  case SCREEN:
    //    return 0xff000000 | ((((tmpR1 + tmpR2 - ((tmpR1 * tmpR2) >> 8)) <<16)&0x00ff0000) | ((((tmpG1 + tmpG2 -(tmpG1 * tmpG2) >> 8))<<8)&0x0000ff00) | ((( (tmpB1+tmpB2 - (tmpB1 * tmpB2)) >> 8))&0x000000ff);


    tmpR2 = (((tmpR2 * alpha2 * layeralpha)>>16))&0x000000ff;
    tmpG2 = (((tmpG2 * alpha2 * layeralpha)>>16))&0x000000ff;
    tmpB2 = (((tmpB2 * alpha2 * layeralpha)>>16))&0x000000ff;


    return 0xff000000 | (~(
			   ((((((~tmpR1)&0x000000ff) * ((~tmpR2)&0x000000ff)) >> 8)<<16)&0x00ff0000) 
			   | ((((((~tmpG1)&0x000000ff) * ((~tmpG2)&0x000000ff)) >> 8)<<8)&0x0000ff00) 
			   | ((((((~tmpB1)&0x000000ff) * ((~tmpB2)&0x000000ff)) >> 8))&0x000000ff)
			   ));

  case ADD:

    tmpR1 = tmpR1 + ((tmpR2 * alpha2 * layeralpha)>>16);
    if(tmpR1 > 255)
      tmpR1 = 255;
    tmpG1 = tmpG1 + ((tmpG2 * alpha2 * layeralpha)>>16);
    if(tmpG1 > 255)
      tmpG1 = 255;
    tmpB1 = tmpB1 + ((tmpB2 * alpha2 * layeralpha)>>16);
    if(tmpB1 > 255)
      tmpB1 = 255;
    

    return 0xff000000 | ((tmpR1<<16)&0x00ff0000) | ((tmpG1<<8)&0x0000ff00) | (tmpB1&0x000000ff);

  case LIGHTEN:

    if(tmpR2 >= tmpR1)
      tmpR1 = tmpR1 - (((tmpR1 - tmpR2) * alpha2 * layeralpha)>>16);
    if(tmpG2 >= tmpG1)
      tmpG1 = tmpG1 - (((tmpG1 - tmpG2) * alpha2 * layeralpha)>>16);
    if(tmpB2 >= tmpB1)
      tmpB1 = tmpB1 - (((tmpB1 - tmpB2) * alpha2 * layeralpha)>>16);


    return 0xff000000 | ((tmpR1<<16)&0x00ff0000) | ((tmpG1<<8)&0x0000ff00) | (tmpB1&0x000000ff);

  case DARKEN:
    if(tmpR2 < tmpR1)
      tmpR1 = tmpR1 - (((tmpR1 - tmpR2) * alpha2 * layeralpha)>>16);
    if(tmpG2 < tmpG1)
      tmpG1 = tmpG1 - (((tmpG1 - tmpG2) * alpha2 * layeralpha)>>16);
    if(tmpB2 < tmpB1)
      tmpB1 = tmpB1 - (((tmpB1 - tmpB2) * alpha2 * layeralpha)>>16);


    return 0xff000000 | ((tmpR1<<16)&0x00ff0000) | ((tmpG1<<8)&0x0000ff00) | (tmpB1&0x000000ff);


  case DIFFERENT:
    tmpR2 = (((tmpR2 * alpha2 * layeralpha)>>16))&0x000000ff;
    tmpG2 = (((tmpG2 * alpha2 * layeralpha)>>16))&0x000000ff;
    tmpB2 = (((tmpB2 * alpha2 * layeralpha)>>16))&0x000000ff;

    if(tmpR1 >= tmpR2)
      tmpR1 = tmpR1-tmpR2;
    else if(tmpR1 < tmpR2)
      tmpR1 = tmpR2-tmpR1;
    if(tmpG1 >= tmpG2)
      tmpG1 = tmpG1-tmpG2;
    else if(tmpG1 < tmpG2)
      tmpG1 = tmpG2-tmpG1;
    if(tmpB1 >= tmpB2)
      tmpB1 = tmpB1-tmpB2;
    else if(tmpB1 < tmpB2)
      tmpB1 = tmpB2-tmpB1;

    return 0xff000000 | ((tmpR1<<16)&0x00ff0000) | ((tmpG1<<8)&0x0000ff00) | (tmpB1&0x000000ff);


  case HUE:
    if((ope2&0x00ffffff) == 0x00ffffff){
      return ope1;
    }
    else if((ope1&0x00ffffff) == 0x00000000){
      return ope1;
    }
    else if((ope1&0x00ffffff) == 0x00ffffff){
      return ope1;
    }

    tmpR11 = tmpR1;
    tmpG11 = tmpG1;
    tmpB11 = tmpB1;
    CalcHue(&tmpR1,&tmpG1,&tmpB1,tmpR2,tmpG2,tmpB2);
    tmpR1 = (tmpR11 - (((tmpR11 - tmpR1) * alpha2 * layeralpha)>>16))&0x000000ff;
    tmpG1 = (tmpG11 - (((tmpG11 - tmpG1) * alpha2 * layeralpha)>>16))&0x000000ff;
    tmpB1 = (tmpB11 - (((tmpB11 - tmpB1) * alpha2 * layeralpha)>>16))&0x000000ff;


    return 0xff000000 | ((tmpR1<<16)&0x00ff0000) | ((tmpG1<<8)&0x0000ff00) | (tmpB1&0x000000ff);


  case SATURATION:
    if((ope2&0x00ffffff) == 0x00ffffff){
      return ope1;
    }
    else if((ope1&0x00ffffff) == 0x00ffffff){
      return ope1;
    }

    tmpR11 = tmpR1;
    tmpG11 = tmpG1;
    tmpB11 = tmpB1;
    CalcSaturation(&tmpR1,&tmpG1,&tmpB1,tmpR2,tmpG2,tmpB2);
    tmpR1 = (tmpR11 - (((tmpR11 - tmpR1) * alpha2 * layeralpha)>>16))&0x000000ff;
    tmpG1 = (tmpG11 - (((tmpG11 - tmpG1) * alpha2 * layeralpha)>>16))&0x000000ff;
    tmpB1 = (tmpB11 - (((tmpB11 - tmpB1) * alpha2 * layeralpha)>>16))&0x000000ff;

    return 0xff000000 | ((tmpR1<<16)&0x00ff0000) | ((tmpG1<<8)&0x0000ff00) | (tmpB1&0x000000ff);


  case COLOR:
    if((ope1&0x00ffffff) == 0x00ffffff){
      return ope1;
    }

    tmpR11 = tmpR1;
    tmpG11 = tmpG1;
    tmpB11 = tmpB1;

    // 白地に対するみかけの色

    CalcColor(&tmpR1,&tmpG1,&tmpB1,tmpR2,tmpG2,tmpB2);

    tmpR1 = (tmpR11 - (((tmpR11 - tmpR1) * alpha2 * layeralpha)>>16))&0x000000ff;
    tmpG1 = (tmpG11 - (((tmpG11 - tmpG1) * alpha2 * layeralpha)>>16))&0x000000ff;
    tmpB1 = (tmpB11 - (((tmpB11 - tmpB1) * alpha2 * layeralpha)>>16))&0x000000ff;

    return 0xff000000 | ((tmpR1<<16)&0x00ff0000) | ((tmpG1<<8)&0x0000ff00) | (tmpB1&0x000000ff);


  case BRIGHTNESS:
    if((ope1&0x00ffffff) == 0x00ffffff){
      return ope1;
    }

    tmpR11 = tmpR1;
    tmpG11 = tmpG1;
    tmpB11 = tmpB1;
    CalcBrightness(&tmpR1,&tmpG1,&tmpB1,tmpR2,tmpG2,tmpB2);
    tmpR1 = (tmpR11 - (((tmpR11 - tmpR1) * alpha2 * layeralpha)>>16))&0x000000ff;
    tmpG1 = (tmpG11 - (((tmpG11 - tmpG1) * alpha2 * layeralpha)>>16))&0x000000ff;
    tmpB1 = (tmpB11 - (((tmpB11 - tmpB1) * alpha2 * layeralpha)>>16))&0x000000ff;

    return 0xff000000 | ((tmpR1<<16)&0x00ff0000) | ((tmpG1<<8)&0x0000ff00) | (tmpB1&0x000000ff);


  case BURN:
    
    tmpR2 = (255 - (((255 - tmpR2) * alpha2 * layeralpha)>>16))&0x000000ff;
    tmpG2 = (255 - (((255 - tmpG2) * alpha2 * layeralpha)>>16))&0x000000ff;
    tmpB2 = (255 - (((255 - tmpB2) * alpha2 * layeralpha)>>16))&0x000000ff;


    if(tmpR2 <= 0)
      tmpR11 = 0;
    else if(tmpR1 >= 255)
      tmpR11 = 255;
    else
      tmpR11 = 255 - ((((255-tmpR1)<<8)/tmpR2));

    if(tmpR11 <= 0)
      tmpR1 = 0;
    else if(tmpR11 >= 255)
      tmpR1 = 255;
    else
      tmpR1 = (unsigned int)tmpR11;

    if(tmpG2 <= 0)
      tmpG11 = 0;
    else if(tmpG1 >= 255)
      tmpG11 = 255;
    else
      tmpG11 = 255 - ((((255-tmpG1)<<8)/tmpG2));
    if(tmpG11 <= 0)
      tmpG1 = 0;
    else if(tmpG11 >= 255)
      tmpG1 = 255;
    else
      tmpG1 = (unsigned int)tmpG11;

    if(tmpB2 <= 0)
      tmpB11 = 0;
    else if(tmpB1 >= 255)
      tmpB11 = 255;
    else
      tmpB11 = 255 - ((((255-tmpB1)<<8)/tmpB2));
    if(tmpB11 <= 0)
      tmpB1 = 0;
    else if(tmpB11 >= 255)
      tmpB1 = 255;
    else
      tmpB1 = (unsigned int)tmpB11;
    
    return 0xff000000 | ((tmpR1<<16)&0x00ff0000) | ((tmpG1<<8)&0x0000ff00) | (tmpB1&0x000000ff);

  case DODGE:

    tmpR2 = (((tmpR2 * alpha2 * layeralpha)>>16))&0x000000ff;
    tmpG2 = (((tmpG2 * alpha2 * layeralpha)>>16))&0x000000ff;
    tmpB2 = (((tmpB2 * alpha2 * layeralpha)>>16))&0x000000ff;


    if(tmpR2 == 255)
      tmpR1 = 0x000000ff;
    else
      //      tmpR1 = ((tmpR1<<8)*div255[(255-tmpR2)]);
      tmpR1 = ((tmpR1<<8)/(255-tmpR2));
    if(tmpR1 > 255)
      tmpR1 = 255;
    if(tmpG2 == 255)
      tmpG1 = 0x000000ff;
    else
      tmpG1 = ((tmpG1<<8)/(255-tmpG2));
    if(tmpG1 > 255)
      tmpG1 = 255;
    if(tmpB2 == 255)
      tmpB1 = 0x000000ff;
    else
      tmpB1 = ((tmpB1<<8)/(255-tmpB2));
    if(tmpB1 > 255)
      tmpB1 = 255;

    return 0xff000000 | ((tmpR1<<16)&0x00ff0000) | ((tmpG1<<8)&0x0000ff00) | (tmpB1&0x000000ff);


  case MASK:
    return ope1;


  default:
    return ope1;

  }
}


void Canvas::paintEvent( QPaintEvent * )
{

  QDirectPainter dp(this);
  unsigned short *frameBuffer = (unsigned short *)dp.frameBuffer();
  int tmpy, tmpx;
  unsigned int tmpColor = 0;


  // clover.cppのresizeEventだけでは取りこぼす
  /*  if(windowDirection == YOKO && QApplication::desktop()->width() == 480){
      windowDirection = TATE;

      clover->resizeEvent(0);

      return;
      }*/


  if(windowDirection ==YOKO){
    return;
  }



  //フレームバッファは常に縦640x横480で取得される
  //dp.numRects()は常に1と考えていい

  //  fprintf(stderr, "alpha : %d\n",layerAlpha2[1]);

  if(zoomStyle == NORMAL){
    for(int i=0; i<dp.numRects(); i++){
      QRect r = dp.rect(i);
      for(int y=r.top(); y<=r.bottom(); y++){
	for(int x=r.left(); x<=r.right(); x++){
	  //lineStep()はcharで数えるため2で割る
	  //pixel()は24bitカラーを返すので16bitカラーへ変換
	  if(((y)+canvasTop) >= SCREEN_HEIGHT 
	     || ((y)+canvasTop) < 0
	     || ((x)+canvasLeft)  >= SCREEN_WIDTH
	     || ((x)+canvasLeft) < 0){
	    bgImage->setPixel(x, y, 0xff808080);
	    if(useColorMatrix){
	      frameBuffer[y * 480 + x] = 
		(( colorMatrix[0][((bgImage->pixel(x,y) & 0x00ff0000)>>16) & 0x000000ff] << 8) & 0xf800 )
		| (( colorMatrix[1][((bgImage->pixel(x,y) & 0x0000ff00)>>8) & 0x000000ff] << 3) & 0x07e0)
		| (( colorMatrix[2][((bgImage->pixel(x,y) & 0x000000ff)) & 0x000000ff] >> 3) & 0x001f);
	    }
	    else{
	      frameBuffer[y * 480 + x] = 
		((   (bgImage->pixel(x,y) & 0x00ff0000) >> 8) & 0xf800)
		| (((bgImage->pixel(x,y) & 0x0000ff00) >> 5) & 0x07e0)
		| (((bgImage->pixel(x,y) & 0x000000ff) >> 3) & 0x001f);
	    }
	  }
	  else{
	    if(layerVisible[0]){
	      tmpColor = layer[0]->pixel((x)+canvasLeft,(y)+canvasTop);
	    }
	    else{
	      if(x%20 < 10 && y%20 < 10){
		tmpColor = 0xffffffff;
	      }
	      else if(x%20 >= 10 && y%20 >= 10){
		tmpColor = 0xffffffff;
	      }
	      else{
		tmpColor = 0xffcccccc;
	      }
	    }


	    if(0 == currentlayer && bokeashiFlag && penPressFlag){
	      if(tool != ERASER && !kasanenuriFlag)
		tmpColor = calc(drawMode, tmpColor, *(drawChecker+(y+canvasTop)*SCREEN_WIDTH+x+canvasLeft), penalpha2);
	      else
		tmpColor = drawPoint(x+canvasLeft,y+canvasTop,*(drawChecker+(y+canvasTop)*SCREEN_WIDTH+x+canvasLeft), -1, true);
	    }
	    for(int m=1; m<layernum; m++){
	      if(layerVisible[m]){
		int layerColor = layer[m]->pixel((x)+canvasLeft,(y)+canvasTop);
		if(m == currentlayer && bokeashiFlag && penPressFlag){
		  layerColor = drawPoint(x+canvasLeft,y+canvasTop,*(drawChecker+(y+canvasTop)*SCREEN_WIDTH+x+canvasLeft), -1, true);
		}
		tmpColor = calc(layerMode[m], tmpColor, layerColor, layerAlpha2[m]);
	      }
	    }

	    //	    bgImage->setPixel(x+canvasLeft, y+canvasTop, tmpColor);
	    bgImage->setPixel(x, y, tmpColor);
	    
	    if(useColorMatrix){
	      frameBuffer[y * 480 + x] = 
		(( colorMatrix[0][((tmpColor & 0x00ff0000)>>16) & 0x000000ff] << 8) & 0xf800 )
		| (( colorMatrix[1][((tmpColor & 0x0000ff00)>>8) & 0x000000ff] << 3) & 0x07e0)
		| (( colorMatrix[2][((tmpColor & 0x000000ff)) & 0x000000ff] >> 3) & 0x001f);
	    }
	    else{
	      frameBuffer[y * 480 + x] = 
		(( (tmpColor & 0x00ff0000) >> 8) & 0xf800)
		| (((tmpColor & 0x0000ff00) >> 5) & 0x07e0)
		| (((tmpColor & 0x000000ff) >> 3) & 0x001f);
	    }

	  }
	}
      }
    }
  }
  else if(zoomStyle == LUPE){
    if(!repaintRectFlag){
      // LUPE 通常はこのセクション．drawLine直後のみrepaintRectFlagがtrue;

      for(int i=0; i<dp.numRects(); i++){
	QRect r = dp.rect(i);
	for(int y=r.top(); y<=r.bottom(); y++){
	  for(int x=r.left(); x<=r.right(); x++){
	    //lineStep()はcharで数えるため2で割る
	    //pixel()は24bitカラーを返すので16bitカラーへ変換

	    if(canvasType == VGA){
	      if(((y)+canvasTop) >= SCREEN_HEIGHT 
		 || ((y)+canvasTop) < 0
		 || ((x)+canvasLeft)  >= SCREEN_WIDTH
		 || ((x)+canvasLeft) < 0){
		bgImage->setPixel(x, y, 0xff808080);
		if(useColorMatrix){
		  frameBuffer[y * 480 + x] = 
		    (( colorMatrix[0][((bgImage->pixel(x,y) & 0x00ff0000)>>16) & 0x000000ff] << 8) & 0xf800 )
		    | (( colorMatrix[1][((bgImage->pixel(x,y) & 0x0000ff00)>>8) & 0x000000ff] << 3) & 0x07e0)
		    | (( colorMatrix[2][((bgImage->pixel(x,y) & 0x000000ff)) & 0x000000ff] >> 3) & 0x001f);
		}
		else{
		  frameBuffer[y * 480 + x] = 
		    ((   (bgImage->pixel(x,y) & 0x00ff0000) >> 8) & 0xf800)
		    | (((bgImage->pixel(x,y) & 0x0000ff00) >> 5) & 0x07e0)
		    | (((bgImage->pixel(x,y) & 0x000000ff) >> 3) & 0x001f);
		}
	      }
	      else{

		if(layerVisible[0]){
		  tmpColor = layer[0]->pixel((x)+canvasLeft,(y)+canvasTop);
		}
		else{
		  if(x%20 < 10 && y%20 < 10){
		    tmpColor = 0xffffffff;
		  }
		  else if(x%20 >= 10 && y%20 >= 10){
		    tmpColor = 0xffffffff;
		  }
		  else{
		    tmpColor = 0xffcccccc;
		  }
		}

		if(0 == currentlayer && bokeashiFlag && penPressFlag){
		  if(tool != ERASER && !kasanenuriFlag)
		    tmpColor = calc(drawMode, tmpColor, *(drawChecker+(y+canvasTop)*SCREEN_WIDTH+x+canvasLeft), penalpha2);
		  else
		    tmpColor = drawPoint(x+canvasLeft,y+canvasTop,*(drawChecker+(y+canvasTop)*SCREEN_WIDTH+x+canvasLeft), -1, true);

		}
		for(int m=1; m<layernum; m++){
		  if(layerVisible[m]){
		    int layerColor = layer[m]->pixel((x)+canvasLeft,(y)+canvasTop);
		    if(m == currentlayer && bokeashiFlag && penPressFlag){
		      layerColor = drawPoint(x+canvasLeft,y+canvasTop,*(drawChecker+(y+canvasTop)*SCREEN_WIDTH+x+canvasLeft), -1, true);
		    }
		    tmpColor = calc(layerMode[m], tmpColor, layerColor, layerAlpha2[m]);

		  }
		}
		//		bgImage->setPixel(x+canvasLeft, y+canvasTop, tmpColor);
		bgImage->setPixel(x, y, tmpColor);

		if(useColorMatrix){
		  frameBuffer[y * 480 + x] = 
		    (( colorMatrix[0][((tmpColor & 0x00ff0000)>>16) & 0x000000ff] << 8) & 0xf800 )
		    | (( colorMatrix[1][((tmpColor & 0x0000ff00)>>8) & 0x000000ff] << 3) & 0x07e0)
		    | (( colorMatrix[2][((tmpColor & 0x000000ff)) & 0x000000ff] >> 3) & 0x001f);
		}
		else{
		  frameBuffer[y * 480 + x] = 
		    (( (tmpColor & 0x00ff0000) >> 8) & 0xf800)
		    | (((tmpColor & 0x0000ff00) >> 5) & 0x07e0)
		    | (((tmpColor & 0x000000ff) >> 3) & 0x001f);
		}

	      }
	    }
	    else if(canvasType == QVGA){

	      if(layerVisible[0]){
		tmpColor = layer[0]->pixel((x>>zoomScale),(y>>zoomScale));
	      }
	      else{
		if(x%20 < 10 && y%20 < 10){
		  tmpColor = 0xffffffff;
		}
		else if(x%20 >= 10 && y%20 >= 10){
		  tmpColor = 0xffffffff;
		}
		else{
		  tmpColor = 0xffcccccc;
		}
	      }

	      if(0 == currentlayer && bokeashiFlag && penPressFlag){
		if(tool != ERASER && !kasanenuriFlag)
		  tmpColor = calc(drawMode, tmpColor, *(drawChecker+(y>>zoomScale)*SCREEN_WIDTH+(x>>zoomScale)), penalpha2);
		else
		  tmpColor = drawPoint(x>>zoomScale,y>>zoomScale,*(drawChecker+(y>>zoomScale)*SCREEN_WIDTH+(x>>zoomScale)), -1, true);
	      }
	      for(int m=1; m<layernum; m++){
		if(layerVisible[m]){
		  int layerColor = layer[m]->pixel((x>>zoomScale),(y>>zoomScale));
		  if(m == currentlayer && bokeashiFlag && penPressFlag){
		    layerColor = drawPoint(x>>zoomScale,y>>zoomScale,*(drawChecker+(y>>zoomScale)*SCREEN_WIDTH+(x>>zoomScale)), -1, true);
		  }
		  tmpColor = calc(layerMode[m],tmpColor, layerColor, layerAlpha2[m]);
		}
	      }

	      //	      bgImage->setPixel((x>>zoomScale), (y>>zoomScale), tmpColor);

	      int tmpX = ((int)(x/2))*2;//丸め
	      int tmpY = ((int)(y/2))*2;
	      if(tmpX > 478)
		tmpX = 478;
	      if(tmpY > 638)
		tmpY = 638;
	      bgImage->setPixel(tmpX, tmpY, tmpColor);
	      bgImage->setPixel(tmpX+1, tmpY, tmpColor);
	      bgImage->setPixel(tmpX, tmpY+1, tmpColor);
	      bgImage->setPixel(tmpX+1, tmpY+1, tmpColor);


	      if(useColorMatrix){
		frameBuffer[y * 480 + x] = 
		  (( colorMatrix[0][((tmpColor & 0x00ff0000)>>16) & 0x000000ff] << 8) & 0xf800 )
		  | (( colorMatrix[1][((tmpColor & 0x0000ff00)>>8) & 0x000000ff] << 3) & 0x07e0)
		  | (( colorMatrix[2][((tmpColor & 0x000000ff)) & 0x000000ff] >> 3) & 0x001f);
	      }
	      else{
		frameBuffer[y * 480 + x] = 
		  (( (tmpColor & 0x00ff0000) >> 8) & 0xf800)
		  | (((tmpColor & 0x0000ff00) >> 5) & 0x07e0)
		  | (((tmpColor & 0x000000ff) >> 3) & 0x001f);
	      }

	    }
	  }
	}
      }
    }
    else{
      // drawLineの直後はこちらのセクション
      repaintRectFlag = false;


      for(int y=repaint_top; y< repaint_top+repaint_height; y++){
	for(int x=repaint_left; x< repaint_left+repaint_width; x++){
	  //カンバスの描画

	  if(layerVisible[0]){
	    tmpColor = layer[0]->pixel((x)+canvasLeft,(y)+canvasTop);
	  }
	  else{
	    if(x%20 < 10 && y%20 < 10){
	      tmpColor = 0xffffffff;
	    }
	    else if(x%20 >= 10 && y%20 >= 10){
	      tmpColor = 0xffffffff;
	    }
	    else{
	      tmpColor = 0xffcccccc;
	    }
	  }

	  if(0 == currentlayer && bokeashiFlag && penPressFlag){
	      if(canvasType == VGA){
		if(tool != ERASER  && !kasanenuriFlag)
		  tmpColor = calc(drawMode, tmpColor, *(drawChecker+(y+canvasTop)*SCREEN_WIDTH+x+canvasLeft), penalpha2);
		else
		  tmpColor = drawPoint(x+canvasLeft,y+canvasTop,*(drawChecker+(y+canvasTop)*SCREEN_WIDTH+x+canvasLeft), -1, true);
	      }
	      else{
		if(tool != ERASER  && !kasanenuriFlag)
		  tmpColor = calc(drawMode, tmpColor, *(drawChecker+(y)*SCREEN_WIDTH+x), penalpha2);
		else
		  tmpColor = drawPoint(x,y,*(drawChecker+(y)*SCREEN_WIDTH+x), -1, true);
	      }
	  }
	  for(int m=1; m<layernum; m++){
	    if(layerVisible[m]){
	      if(canvasType == VGA){
		int layerColor = layer[m]->pixel((x)+canvasLeft,(y)+canvasTop);
		if(m == currentlayer && bokeashiFlag && penPressFlag){
		  layerColor = drawPoint(x+canvasLeft,y+canvasTop,*(drawChecker+(y+canvasTop)*SCREEN_WIDTH+x+canvasLeft), -1, true);
		}
		tmpColor = calc(layerMode[m], tmpColor, layerColor, layerAlpha2[m]);
	      }
	      else if(canvasType == QVGA){
		int layerColor = layer[m]->pixel(x,y);
		if(m == currentlayer && bokeashiFlag && penPressFlag){
		  layerColor = drawPoint(x,y,*(drawChecker+(y)*SCREEN_WIDTH+x), -1, true);
		}
		tmpColor = calc(layerMode[m],tmpColor, layerColor, layerAlpha2[m]);
	      }
	    }
   	  }
	  //	  bgImage->setPixel(x+canvasLeft, y+canvasTop, tmpColor);

	  if(canvasType == VGA){
	    bgImage->setPixel(x+canvasLeft, y+canvasTop, tmpColor);
	  }
	  else if(canvasType == QVGA){
	    int tmpX = x*2;
	    int tmpY = y*2;
	    if(tmpX <= 479 && tmpY <= 639)
	      bgImage->setPixel(tmpX, tmpY, tmpColor);
	    if(tmpX+1 <= 479 && tmpY <= 639)
	      bgImage->setPixel(tmpX+1, tmpY, tmpColor);
	    if(tmpX <= 479 && tmpY+1 <= 639)
	      bgImage->setPixel(tmpX, tmpY+1, tmpColor);
	    if(tmpX+1 <= 479 && tmpY+1 <= 639)
	      bgImage->setPixel(tmpX+1, tmpY+1, tmpColor);
	  }


	  if(canvasType == VGA){
	    if((y<lupePanel->y() || y>=lupePanel->y()+246 || x<lupePanel->x() || x>=lupePanel->x()+227)
	       && ((y<pensizePanel->y() || y>=pensizePanel->y()+198 || x<pensizePanel->x() || x>=pensizePanel->x()+65) || !pensizePanel->isVisible())
	       && ((y<meshPanel->y() || y>=meshPanel->y()+201 || x<meshPanel->x() || x>=meshPanel->x()+65)  || !meshPanel->isVisible())
	       && ((y<palettePanel->y() || y>=palettePanel->y()+201 || x<palettePanel->x() || x>=palettePanel->x()+65)  || !palettePanel->isVisible())
	       && ((y<scalePanel->y() || y>=scalePanel->y()+111 || x<scalePanel->x() || x>=scalePanel->x()+65)  || !scalePanel->isVisible())
	       && ((y<toolPanel->y() || y>=toolPanel->y()+169 || x<toolPanel->x() || x>=toolPanel->x()+65)  || !toolPanel->isVisible())
	       && ((y<selectPanel->y() || y>=selectPanel->y()+selectPanel->height() || x<selectPanel->x() || x>=selectPanel->x()+selectPanel->width())  || !selectPanel->isVisible())
	       && ((y<toolbar->y() || y>=toolbar->y()+62)  ||  !toolbar->isVisible())
	       && ((y<huebar->y() || y>=huebar->y()+20) || !huebar->isVisible())

	       && ((y<layerPanel->y() || y>=layerPanel->y()+layerPanel->height() || x<layerPanel->x() || x>=layerPanel->x()+148) || !layerPanel->isVisible())

	       && ((y<layerModePanel->y() || y>=layerModePanel->y()+layerModePanel->height() || x<layerModePanel->x() || x>=layerModePanel->x()+layerModePanel->width()) || !layerModePanel->isVisible())
	       && ((y<drawModePanel->y() || y>=drawModePanel->y()+drawModePanel->height() || x<drawModePanel->x() || x>=drawModePanel->x()+drawModePanel->width()) || !drawModePanel->isVisible())

	       && ((y<fileMenuPanel->y() || y>=fileMenuPanel->y()+fileMenuPanel->height() || x<fileMenuPanel->x() || x>=fileMenuPanel->x()+fileMenuPanel->width()) || !fileMenuPanel->isVisible())
	       ){

	      if(useColorMatrix){
		frameBuffer[y * 480 + x] = 
		  (( colorMatrix[0][((tmpColor & 0x00ff0000)>>16) & 0x000000ff] << 8) & 0xf800 )
		  | (( colorMatrix[1][((tmpColor & 0x0000ff00)>>8) & 0x000000ff] << 3) & 0x07e0)
		  | (( colorMatrix[2][((tmpColor & 0x000000ff)) & 0x000000ff] >> 3) & 0x001f);
	      }
	      else{
		frameBuffer[y * 480 + x] = 
		  (( (tmpColor & 0x00ff0000) >> 8) & 0xf800)
		  | (((tmpColor & 0x0000ff00) >> 5) & 0x07e0)
		  | (((tmpColor & 0x000000ff) >> 3) & 0x001f);
	      }

	    }

	  }
	  else if(canvasType == QVGA){

	    if(((y*2)<lupePanel->y() || (y*2)>=lupePanel->y()+246 || (x*2)<lupePanel->x() || (x*2)>=lupePanel->x()+227)
	       && (((y*2)<pensizePanel->y() || (y*2)>=pensizePanel->y()+198 || (x*2)<pensizePanel->x() || (x*2)>=pensizePanel->x()+65) || !pensizePanel->isVisible())
	       && (((y*2)<meshPanel->y() || (y*2)>=meshPanel->y()+201 || (x*2)<meshPanel->x() || (x*2)>=meshPanel->x()+65)  || !meshPanel->isVisible())
	       && (((y*2)<palettePanel->y() || (y*2)>=palettePanel->y()+201 || (x*2)<palettePanel->x() || (x*2)>=palettePanel->x()+65)  || !palettePanel->isVisible())
	       && (((y*2)<scalePanel->y() || (y*2)>=scalePanel->y()+111 || (x*2)<scalePanel->x() || (x*2)>=scalePanel->x()+65)  || !scalePanel->isVisible())
	       && (((y*2)<toolPanel->y() || (y*2)>=toolPanel->y()+169 || (x*2)<toolPanel->x() || (x*2)>=toolPanel->x()+65)  || !toolPanel->isVisible())
	       && (((y*2)<selectPanel->y() || (y*2)>=selectPanel->y()+selectPanel->height() || (x*2)<selectPanel->x() || (x*2)>=selectPanel->x()+selectPanel->width())  || !selectPanel->isVisible())
	       && (((y*2)<toolbar->y() || (y*2)>=toolbar->y()+62)  ||  !toolbar->isVisible())
	       && (((y*2)<huebar->y() || (y*2)>=huebar->y()+20) || !huebar->isVisible())
	       && (((y*2)<layerPanel->y() || (y*2)>=layerPanel->y()+layerPanel->height() || (x*2)<layerPanel->x() || (x*2)>=layerPanel->x()+148) || !layerPanel->isVisible())
	       && (((y*2)<layerModePanel->y() || (y*2)>=layerModePanel->y()+layerModePanel->height() || (x*2)<layerModePanel->x() || (x*2)>=layerModePanel->x()+layerModePanel->width()) || !layerModePanel->isVisible())
	       && (((y*2)<drawModePanel->y() || (y*2)>=drawModePanel->y()+drawModePanel->height() || (x*2)<drawModePanel->x() || (x*2)>=drawModePanel->x()+drawModePanel->width()) || !drawModePanel->isVisible())
	       && (((y*2)<fileMenuPanel->y() || (y*2)>=fileMenuPanel->y()+fileMenuPanel->height() || (x*2)<fileMenuPanel->x() || (x*2)>=fileMenuPanel->x()+fileMenuPanel->width()) || !fileMenuPanel->isVisible())
	       ){
	      if(useColorMatrix){
		if(y*2<640 && x*2<480){
		  frameBuffer[y*2 * 480 + x*2] = 
		    (( colorMatrix[0][((tmpColor & 0x00ff0000)>>16) & 0x000000ff] << 8) & 0xf800 )
		    | (( colorMatrix[1][((tmpColor & 0x0000ff00)>>8) & 0x000000ff] << 3) & 0x07e0)
		    | (( colorMatrix[2][((tmpColor & 0x000000ff)) & 0x000000ff] >> 3) & 0x001f);
		}
	      }
	      else{
		if(y*2<640 && x*2<480){
		  frameBuffer[y*2 * 480 + x*2] = 
		    (( (tmpColor & 0x00ff0000) >> 8) & 0xf800)
		    | (((tmpColor & 0x0000ff00) >> 5) & 0x07e0)
		    | (((tmpColor & 0x000000ff) >> 3) & 0x001f);
		} 
	      }
	    }


	    if(((y*2+1)<lupePanel->y() || (y*2+1)>=lupePanel->y()+246 || (x*2)<lupePanel->x() || (x*2)>=lupePanel->x()+227)
	       && (((y*2+1)<pensizePanel->y() || (y*2+1)>=pensizePanel->y()+198 || (x*2)<pensizePanel->x() || (x*2)>=pensizePanel->x()+65) || !pensizePanel->isVisible())
	       && (((y*2+1)<meshPanel->y() || (y*2+1)>=meshPanel->y()+201 || (x*2)<meshPanel->x() || (x*2)>=meshPanel->x()+65)  || !meshPanel->isVisible())
	       && (((y*2+1)<palettePanel->y() || (y*2+1)>=palettePanel->y()+201 || (x*2)<palettePanel->x() || (x*2)>=palettePanel->x()+65)  || !palettePanel->isVisible())
	       && (((y*2+1)<scalePanel->y() || (y*2+1)>=scalePanel->y()+111 || (x*2)<scalePanel->x() || (x*2)>=scalePanel->x()+65)  || !scalePanel->isVisible())
	       && (((y*2+1)<toolPanel->y() || (y*2+1)>=toolPanel->y()+169 || (x*2)<toolPanel->x() || (x*2)>=toolPanel->x()+65)  || !toolPanel->isVisible())
	       && (((y*2+1)<selectPanel->y() || (y*2+1)>=selectPanel->y()+selectPanel->height() || (x*2)<selectPanel->x() || (x*2)>=selectPanel->x()+selectPanel->width())  || !selectPanel->isVisible())
	       && (((y*2+1)<toolbar->y() || (y*2+1)>=toolbar->y()+62)  ||  !toolbar->isVisible())
	       && (((y*2+1)<huebar->y() || (y*2+1)>=huebar->y()+20) || !huebar->isVisible())
	       && (((y*2+1)<layerPanel->y() || (y*2+1)>=layerPanel->y()+layerPanel->height() || (x*2)<layerPanel->x() || (x*2)>=layerPanel->x()+148) || !layerPanel->isVisible())
	       && (((y*2+1)<layerModePanel->y() || (y*2+1)>=layerModePanel->y()+layerModePanel->height() || (x*2)<layerModePanel->x() || (x*2)>=layerModePanel->x()+layerModePanel->width()) || !layerModePanel->isVisible())
	       && (((y*2+1)<drawModePanel->y() || (y*2+1)>=drawModePanel->y()+drawModePanel->height() || (x*2)<drawModePanel->x() || (x*2)>=drawModePanel->x()+drawModePanel->width()) || !drawModePanel->isVisible())
	       && (((y*2+1)<fileMenuPanel->y() || (y*2+1)>=fileMenuPanel->y()+fileMenuPanel->height() || (x*2)<fileMenuPanel->x() || (x*2)>=fileMenuPanel->x()+fileMenuPanel->width()) || !fileMenuPanel->isVisible())
	       ){
	      if(useColorMatrix){
		if(y*2+1<640 && x*2<480){
		  frameBuffer[(y*2+1) * 480 + x*2] = 
		    (( colorMatrix[0][((tmpColor & 0x00ff0000)>>16) & 0x000000ff] << 8) & 0xf800 )
		    | (( colorMatrix[1][((tmpColor & 0x0000ff00)>>8) & 0x000000ff] << 3) & 0x07e0)
		    | (( colorMatrix[2][((tmpColor & 0x000000ff)) & 0x000000ff] >> 3) & 0x001f);
		}
	      }
	      else{
		if(y*2+1<640 && x*2<480){
		  frameBuffer[(y*2+1) * 480 + x*2] = 
		    (( (tmpColor & 0x00ff0000) >> 8) & 0xf800)
		    | (((tmpColor & 0x0000ff00) >> 5) & 0x07e0)
		    | (((tmpColor & 0x000000ff) >> 3) & 0x001f);
		} 
	      }
	    }

	    if(((y*2)<lupePanel->y() || (y*2)>=lupePanel->y()+246 || (x*2+1)<lupePanel->x() || (x*2+1)>=lupePanel->x()+227)
	       && (((y*2)<pensizePanel->y() || (y*2)>=pensizePanel->y()+198 || (x*2+1)<pensizePanel->x() || (x*2+1)>=pensizePanel->x()+65) || !pensizePanel->isVisible())
	       && (((y*2)<meshPanel->y() || (y*2)>=meshPanel->y()+201 || (x*2+1)<meshPanel->x() || (x*2+1)>=meshPanel->x()+65)  || !meshPanel->isVisible())
	       && (((y*2)<palettePanel->y() || (y*2)>=palettePanel->y()+201 || (x*2+1)<palettePanel->x() || (x*2+1)>=palettePanel->x()+65)  || !palettePanel->isVisible())
	       && (((y*2)<scalePanel->y() || (y*2)>=scalePanel->y()+111 || (x*2+1)<scalePanel->x() || (x*2+1)>=scalePanel->x()+65)  || !scalePanel->isVisible())
	       && (((y*2)<toolPanel->y() || (y*2)>=toolPanel->y()+169 || (x*2+1)<toolPanel->x() || (x*2+1)>=toolPanel->x()+65)  || !toolPanel->isVisible())
	       && (((y*2)<selectPanel->y() || (y*2)>=selectPanel->y()+selectPanel->height() || (x*2+1)<selectPanel->x() || (x*2+1)>=selectPanel->x()+selectPanel->width())  || !selectPanel->isVisible())
	       && (((y*2)<toolbar->y() || (y*2)>=toolbar->y()+62)  ||  !toolbar->isVisible())
	       && (((y*2)<huebar->y() || (y*2)>=huebar->y()+20) || !huebar->isVisible())
	       && (((y*2)<layerPanel->y() || (y*2)>=layerPanel->y()+layerPanel->height() || (x*2+1)<layerPanel->x() || (x*2+1)>=layerPanel->x()+148) || !layerPanel->isVisible())
	       && (((y*2)<layerModePanel->y() || (y*2)>=layerModePanel->y()+layerModePanel->height() || (x*2+1)<layerModePanel->x() || (x*2+1)>=layerModePanel->x()+layerModePanel->width()) || !layerModePanel->isVisible())
	       && (((y*2)<drawModePanel->y() || (y*2)>=drawModePanel->y()+drawModePanel->height() || (x*2+1)<drawModePanel->x() || (x*2+1)>=drawModePanel->x()+drawModePanel->width()) || !drawModePanel->isVisible())
	       && (((y*2)<fileMenuPanel->y() || (y*2)>=fileMenuPanel->y()+fileMenuPanel->height() || (x*2+1)<fileMenuPanel->x() || (x*2+1)>=fileMenuPanel->x()+fileMenuPanel->width()) || !fileMenuPanel->isVisible())
	       ){
	      if(useColorMatrix){
		if(y*2<640 && x*2+1<480){
		  frameBuffer[y*2 * 480 + x*2+1] = 
		    (( colorMatrix[0][((tmpColor & 0x00ff0000)>>16) & 0x000000ff] << 8) & 0xf800 )
		    | (( colorMatrix[1][((tmpColor & 0x0000ff00)>>8) & 0x000000ff] << 3) & 0x07e0)
		    | (( colorMatrix[2][((tmpColor & 0x000000ff)) & 0x000000ff] >> 3) & 0x001f);
		}
	      }
	      else{
		if(y*2<640 && x*2+1<480){
		  frameBuffer[y*2 * 480 + x*2+1] = 
		    (( (tmpColor & 0x00ff0000) >> 8) & 0xf800)
		    | (((tmpColor & 0x0000ff00) >> 5) & 0x07e0)
		    | (((tmpColor & 0x000000ff) >> 3) & 0x001f);
		} 
	      }
	    }

	    if(((y*2+1)<lupePanel->y() || (y*2+1)>=lupePanel->y()+246 || (x*2+1)<lupePanel->x() || (x*2+1)>=lupePanel->x()+227)
	       && (((y*2+1)<pensizePanel->y() || (y*2+1)>=pensizePanel->y()+198 || (x*2+1)<pensizePanel->x() || (x*2+1)>=pensizePanel->x()+65) || !pensizePanel->isVisible())
	       && (((y*2+1)<meshPanel->y() || (y*2+1)>=meshPanel->y()+201 || (x*2+1)<meshPanel->x() || (x*2+1)>=meshPanel->x()+65)  || !meshPanel->isVisible())
	       && (((y*2+1)<palettePanel->y() || (y*2+1)>=palettePanel->y()+201 || (x*2+1)<palettePanel->x() || (x*2+1)>=palettePanel->x()+65)  || !palettePanel->isVisible())
	       && (((y*2+1)<scalePanel->y() || (y*2+1)>=scalePanel->y()+111 || (x*2+1)<scalePanel->x() || (x*2+1)>=scalePanel->x()+65)  || !scalePanel->isVisible())
	       && (((y*2+1)<toolPanel->y() || (y*2+1)>=toolPanel->y()+169 || (x*2+1)<toolPanel->x() || (x*2+1)>=toolPanel->x()+65)  || !toolPanel->isVisible())
	       && (((y*2+1)<selectPanel->y() || (y*2+1)>=selectPanel->y()+selectPanel->height() || (x*2+1)<selectPanel->x() || (x*2+1)>=selectPanel->x()+selectPanel->width())  || !selectPanel->isVisible())
	       && (((y*2+1)<toolbar->y() || (y*2+1)>=toolbar->y()+62)  ||  !toolbar->isVisible())
	       && (((y*2+1)<huebar->y() || (y*2+1)>=huebar->y()+20) || !huebar->isVisible())
	       && (((y*2+1)<layerPanel->y() || (y*2+1)>=layerPanel->y()+layerPanel->height() || (x*2+1)<layerPanel->x() || (x*2+1)>=layerPanel->x()+148) || !layerPanel->isVisible())
	       && (((y*2+1)<layerModePanel->y() || (y*2+1)>=layerModePanel->y()+layerModePanel->height() || (x*2+1)<layerModePanel->x() || (x*2+1)>=layerModePanel->x()+layerModePanel->width()) || !layerModePanel->isVisible())
	       && (((y*2+1)<drawModePanel->y() || (y*2+1)>=drawModePanel->y()+drawModePanel->height() || (x*2+1)<drawModePanel->x() || (x*2+1)>=drawModePanel->x()+drawModePanel->width()) || !drawModePanel->isVisible())
	       && (((y*2+1)<fileMenuPanel->y() || (y*2+1)>=fileMenuPanel->y()+fileMenuPanel->height() || (x*2+1)<fileMenuPanel->x() || (x*2+1)>=fileMenuPanel->x()+fileMenuPanel->width()) || !fileMenuPanel->isVisible())
	       ){
	      if(useColorMatrix){
		if(y*2+1<640 && x*2+1<480){
		  frameBuffer[(y*2+1) * 480 + x*2+1] = 
		    (( colorMatrix[0][((tmpColor & 0x00ff0000)>>16) & 0x000000ff] << 8) & 0xf800 )
		    | (( colorMatrix[1][((tmpColor & 0x0000ff00)>>8) & 0x000000ff] << 3) & 0x07e0)
		    | (( colorMatrix[2][((tmpColor & 0x000000ff)) & 0x000000ff] >> 3) & 0x001f);
		}
	      }
	      else{
		if(y*2+1<640 && x*2+1<480){
		  frameBuffer[(y*2+1) * 480 + x*2+1] = 
		    (( (tmpColor & 0x00ff0000) >> 8) & 0xf800)
		    | (((tmpColor & 0x0000ff00) >> 5) & 0x07e0)
		    | (((tmpColor & 0x000000ff) >> 3) & 0x001f);
		} 
	      }
	    }



	  }



	  //ルーペの描画
	  tmpy = ((y-lupeTop)<<lupeScale)+20+lupePanel->y();
	  tmpx = ((x-lupeLeft)<<lupeScale)+1+lupePanel->x();

	  if(tmpy >= lupePanel->y()+20 && 
	     tmpy < lupePanel->y()+20+224 &&
	     tmpx >= lupePanel->x()+1 &&
	     tmpx < lupePanel->x()+1+224){


	    unsigned short tmpFrameBuffer;

	    if(useColorMatrix){
	      tmpFrameBuffer =
		(( colorMatrix[0][((tmpColor & 0x00ff0000)>>16) & 0x000000ff] << 8) & 0xf800 )
		| (( colorMatrix[1][((tmpColor & 0x0000ff00)>>8) & 0x000000ff] << 3) & 0x07e0)
		| (( colorMatrix[2][((tmpColor & 0x000000ff)) & 0x000000ff] >> 3) & 0x001f);
	    }
	    else{
	      tmpFrameBuffer =
		(( (tmpColor & 0x00ff0000) >> 8) & 0xf800)
		| (((tmpColor & 0x0000ff00) >> 5) & 0x07e0)
		| (((tmpColor & 0x000000ff) >> 3) & 0x001f);
	    }



	    if(lupeScale == 1){
	      frameBuffer[tmpy *480 + tmpx]
		= frameBuffer[tmpy * 480 + tmpx+1]
		= frameBuffer[(tmpy+1) * 480 + tmpx]
		= frameBuffer[(tmpy+1) * 480 + tmpx+1]
		=  tmpFrameBuffer;

	    }
	    else if(lupeScale == 2){
	      frameBuffer[tmpy *480 + tmpx]
		= frameBuffer[tmpy * 480 + tmpx+1]
		= frameBuffer[tmpy * 480 + tmpx+2]
		= frameBuffer[tmpy * 480 + tmpx+3]
		= frameBuffer[(tmpy+1) *480 + tmpx]
		= frameBuffer[(tmpy+1) * 480 + tmpx+1]
		= frameBuffer[(tmpy+1) * 480 + tmpx+2]
		= frameBuffer[(tmpy+1) * 480 + tmpx+3]
		= frameBuffer[(tmpy+2) *480 + tmpx]
		= frameBuffer[(tmpy+2) * 480 + tmpx+1]
		= frameBuffer[(tmpy+2) * 480 + tmpx+2]
		= frameBuffer[(tmpy+2) * 480 + tmpx+3]
		= frameBuffer[(tmpy+3) *480 + tmpx]
		= frameBuffer[(tmpy+3) * 480 + tmpx+1]
		= frameBuffer[(tmpy+3) * 480 + tmpx+2]
		= frameBuffer[(tmpy+3) * 480 + tmpx+3]
		=  tmpFrameBuffer;


	    }
	    else if(lupeScale == 3){
	      frameBuffer[tmpy * 480 + tmpx]
		= frameBuffer[tmpy * 480 + tmpx+1]
		= frameBuffer[tmpy * 480 + tmpx+2]
		= frameBuffer[tmpy * 480 + tmpx+3]
		= frameBuffer[tmpy * 480 + tmpx+4]
		= frameBuffer[tmpy * 480 + tmpx+5]
		= frameBuffer[tmpy * 480 + tmpx+6]
		= frameBuffer[tmpy * 480 + tmpx+7]
		= frameBuffer[(tmpy+1) * 480 + tmpx]
		= frameBuffer[(tmpy+1) * 480 + tmpx+1]
		= frameBuffer[(tmpy+1) * 480 + tmpx+2]
		= frameBuffer[(tmpy+1) * 480 + tmpx+3]
		= frameBuffer[(tmpy+1) * 480 + tmpx+4]
		= frameBuffer[(tmpy+1) * 480 + tmpx+5]
		= frameBuffer[(tmpy+1) * 480 + tmpx+6]
		= frameBuffer[(tmpy+1) * 480 + tmpx+7]
		= frameBuffer[(tmpy+2) * 480 + tmpx]
		= frameBuffer[(tmpy+2) * 480 + tmpx+1]
		= frameBuffer[(tmpy+2) * 480 + tmpx+2]
		= frameBuffer[(tmpy+2) * 480 + tmpx+3]
		= frameBuffer[(tmpy+2) * 480 + tmpx+4]
		= frameBuffer[(tmpy+2) * 480 + tmpx+5]
		= frameBuffer[(tmpy+2) * 480 + tmpx+6]
		= frameBuffer[(tmpy+2) * 480 + tmpx+7]
		= frameBuffer[(tmpy+3) * 480 + tmpx]
		= frameBuffer[(tmpy+3) * 480 + tmpx+1]
		= frameBuffer[(tmpy+3) * 480 + tmpx+2]
		= frameBuffer[(tmpy+3) * 480 + tmpx+3]
		= frameBuffer[(tmpy+3) * 480 + tmpx+4]
		= frameBuffer[(tmpy+3) * 480 + tmpx+5]
		= frameBuffer[(tmpy+3) * 480 + tmpx+6]
		= frameBuffer[(tmpy+3) * 480 + tmpx+7]
		= frameBuffer[(tmpy+4) * 480 + tmpx]
		= frameBuffer[(tmpy+4) * 480 + tmpx+1]
		= frameBuffer[(tmpy+4) * 480 + tmpx+2]
		= frameBuffer[(tmpy+4) * 480 + tmpx+3]
		= frameBuffer[(tmpy+4) * 480 + tmpx+4]
		= frameBuffer[(tmpy+4) * 480 + tmpx+5]
		= frameBuffer[(tmpy+4) * 480 + tmpx+6]
		= frameBuffer[(tmpy+4) * 480 + tmpx+7]
		= frameBuffer[(tmpy+5) * 480 + tmpx]
		= frameBuffer[(tmpy+5) * 480 + tmpx+1]
		= frameBuffer[(tmpy+5) * 480 + tmpx+2]
		= frameBuffer[(tmpy+5) * 480 + tmpx+3]
		= frameBuffer[(tmpy+5) * 480 + tmpx+4]
		= frameBuffer[(tmpy+5) * 480 + tmpx+5]
		= frameBuffer[(tmpy+5) * 480 + tmpx+6]
		= frameBuffer[(tmpy+5) * 480 + tmpx+7]
		= frameBuffer[(tmpy+6) * 480 + tmpx]
		= frameBuffer[(tmpy+6) * 480 + tmpx+1]
		= frameBuffer[(tmpy+6) * 480 + tmpx+2]
		= frameBuffer[(tmpy+6) * 480 + tmpx+3]
		= frameBuffer[(tmpy+6) * 480 + tmpx+4]
		= frameBuffer[(tmpy+6) * 480 + tmpx+5]
		= frameBuffer[(tmpy+6) * 480 + tmpx+6]
		= frameBuffer[(tmpy+6) * 480 + tmpx+7]
		= frameBuffer[(tmpy+7) * 480 + tmpx]
		= frameBuffer[(tmpy+7) * 480 + tmpx+1]
		= frameBuffer[(tmpy+7) * 480 + tmpx+2]
		= frameBuffer[(tmpy+7) * 480 + tmpx+3]
		= frameBuffer[(tmpy+7) * 480 + tmpx+4]
		= frameBuffer[(tmpy+7) * 480 + tmpx+5]
		= frameBuffer[(tmpy+7) * 480 + tmpx+6]
		= frameBuffer[(tmpy+7) * 480 + tmpx+7]
		=  tmpFrameBuffer;

	    }
	    else if(lupeScale == 4){
	      frameBuffer[tmpy * 480 + tmpx]
		= frameBuffer[tmpy * 480 + tmpx+1]
		= frameBuffer[tmpy * 480 + tmpx+2]
		= frameBuffer[tmpy * 480 + tmpx+3]
		= frameBuffer[tmpy * 480 + tmpx+4]
		= frameBuffer[tmpy * 480 + tmpx+5]
		= frameBuffer[tmpy * 480 + tmpx+6]
		= frameBuffer[tmpy * 480 + tmpx+7]
		= frameBuffer[tmpy * 480 + tmpx+8]
		= frameBuffer[tmpy * 480 + tmpx+9]
		= frameBuffer[tmpy * 480 + tmpx+10]
		= frameBuffer[tmpy * 480 + tmpx+11]
		= frameBuffer[tmpy * 480 + tmpx+12]
		= frameBuffer[tmpy * 480 + tmpx+13]
		= frameBuffer[tmpy * 480 + tmpx+14]
		= frameBuffer[tmpy * 480 + tmpx+15]
		= frameBuffer[(tmpy+1) * 480 + tmpx]
		= frameBuffer[(tmpy+1) * 480 + tmpx+1]
		= frameBuffer[(tmpy+1) * 480 + tmpx+2]
		= frameBuffer[(tmpy+1) * 480 + tmpx+3]
		= frameBuffer[(tmpy+1) * 480 + tmpx+4]
		= frameBuffer[(tmpy+1) * 480 + tmpx+5]
		= frameBuffer[(tmpy+1) * 480 + tmpx+6]
		= frameBuffer[(tmpy+1) * 480 + tmpx+7]
		= frameBuffer[(tmpy+1) * 480 + tmpx+8]
		= frameBuffer[(tmpy+1) * 480 + tmpx+9]
		= frameBuffer[(tmpy+1) * 480 + tmpx+10]
		= frameBuffer[(tmpy+1) * 480 + tmpx+11]
		= frameBuffer[(tmpy+1) * 480 + tmpx+12]
		= frameBuffer[(tmpy+1) * 480 + tmpx+13]
		= frameBuffer[(tmpy+1) * 480 + tmpx+14]
		= frameBuffer[(tmpy+1) * 480 + tmpx+15]

		= frameBuffer[(tmpy+2) * 480 + tmpx]
		= frameBuffer[(tmpy+2) * 480 + tmpx+1]
		= frameBuffer[(tmpy+2) * 480 + tmpx+2]
		= frameBuffer[(tmpy+2) * 480 + tmpx+3]
		= frameBuffer[(tmpy+2) * 480 + tmpx+4]
		= frameBuffer[(tmpy+2) * 480 + tmpx+5]
		= frameBuffer[(tmpy+2) * 480 + tmpx+6]
		= frameBuffer[(tmpy+2) * 480 + tmpx+7]
		= frameBuffer[(tmpy+2) * 480 + tmpx+8]
		= frameBuffer[(tmpy+2) * 480 + tmpx+9]
		= frameBuffer[(tmpy+2) * 480 + tmpx+10]
		= frameBuffer[(tmpy+2) * 480 + tmpx+11]
		= frameBuffer[(tmpy+2) * 480 + tmpx+12]
		= frameBuffer[(tmpy+2) * 480 + tmpx+13]
		= frameBuffer[(tmpy+2) * 480 + tmpx+14]
		= frameBuffer[(tmpy+2) * 480 + tmpx+15]

		= frameBuffer[(tmpy+3) * 480 + tmpx]
		= frameBuffer[(tmpy+3) * 480 + tmpx+1]
		= frameBuffer[(tmpy+3) * 480 + tmpx+2]
		= frameBuffer[(tmpy+3) * 480 + tmpx+3]
		= frameBuffer[(tmpy+3) * 480 + tmpx+4]
		= frameBuffer[(tmpy+3) * 480 + tmpx+5]
		= frameBuffer[(tmpy+3) * 480 + tmpx+6]
		= frameBuffer[(tmpy+3) * 480 + tmpx+7]
		= frameBuffer[(tmpy+3) * 480 + tmpx+8]
		= frameBuffer[(tmpy+3) * 480 + tmpx+9]
		= frameBuffer[(tmpy+3) * 480 + tmpx+10]
		= frameBuffer[(tmpy+3) * 480 + tmpx+11]
		= frameBuffer[(tmpy+3) * 480 + tmpx+12]
		= frameBuffer[(tmpy+3) * 480 + tmpx+13]
		= frameBuffer[(tmpy+3) * 480 + tmpx+14]
		= frameBuffer[(tmpy+3) * 480 + tmpx+15]

		= frameBuffer[(tmpy+4) * 480 + tmpx]
		= frameBuffer[(tmpy+4) * 480 + tmpx+1]
		= frameBuffer[(tmpy+4) * 480 + tmpx+2]
		= frameBuffer[(tmpy+4) * 480 + tmpx+3]
		= frameBuffer[(tmpy+4) * 480 + tmpx+4]
		= frameBuffer[(tmpy+4) * 480 + tmpx+5]
		= frameBuffer[(tmpy+4) * 480 + tmpx+6]
		= frameBuffer[(tmpy+4) * 480 + tmpx+7]
		= frameBuffer[(tmpy+4) * 480 + tmpx+8]
		= frameBuffer[(tmpy+4) * 480 + tmpx+9]
		= frameBuffer[(tmpy+4) * 480 + tmpx+10]
		= frameBuffer[(tmpy+4) * 480 + tmpx+11]
		= frameBuffer[(tmpy+4) * 480 + tmpx+12]
		= frameBuffer[(tmpy+4) * 480 + tmpx+13]
		= frameBuffer[(tmpy+4) * 480 + tmpx+14]
		= frameBuffer[(tmpy+4) * 480 + tmpx+15]

		= frameBuffer[(tmpy+5) * 480 + tmpx]
		= frameBuffer[(tmpy+5) * 480 + tmpx+1]
		= frameBuffer[(tmpy+5) * 480 + tmpx+2]
		= frameBuffer[(tmpy+5) * 480 + tmpx+3]
		= frameBuffer[(tmpy+5) * 480 + tmpx+4]
		= frameBuffer[(tmpy+5) * 480 + tmpx+5]
		= frameBuffer[(tmpy+5) * 480 + tmpx+6]
		= frameBuffer[(tmpy+5) * 480 + tmpx+7]
		= frameBuffer[(tmpy+5) * 480 + tmpx+8]
		= frameBuffer[(tmpy+5) * 480 + tmpx+9]
		= frameBuffer[(tmpy+5) * 480 + tmpx+10]
		= frameBuffer[(tmpy+5) * 480 + tmpx+11]
		= frameBuffer[(tmpy+5) * 480 + tmpx+12]
		= frameBuffer[(tmpy+5) * 480 + tmpx+13]
		= frameBuffer[(tmpy+5) * 480 + tmpx+14]
		= frameBuffer[(tmpy+5) * 480 + tmpx+15]

		= frameBuffer[(tmpy+6) * 480 + tmpx]
		= frameBuffer[(tmpy+6) * 480 + tmpx+1]
		= frameBuffer[(tmpy+6) * 480 + tmpx+2]
		= frameBuffer[(tmpy+6) * 480 + tmpx+3]
		= frameBuffer[(tmpy+6) * 480 + tmpx+4]
		= frameBuffer[(tmpy+6) * 480 + tmpx+5]
		= frameBuffer[(tmpy+6) * 480 + tmpx+6]
		= frameBuffer[(tmpy+6) * 480 + tmpx+7]
		= frameBuffer[(tmpy+6) * 480 + tmpx+8]
		= frameBuffer[(tmpy+6) * 480 + tmpx+9]
		= frameBuffer[(tmpy+6) * 480 + tmpx+10]
		= frameBuffer[(tmpy+6) * 480 + tmpx+11]
		= frameBuffer[(tmpy+6) * 480 + tmpx+12]
		= frameBuffer[(tmpy+6) * 480 + tmpx+13]
		= frameBuffer[(tmpy+6) * 480 + tmpx+14]
		= frameBuffer[(tmpy+6) * 480 + tmpx+15]

		= frameBuffer[(tmpy+7) * 480 + tmpx]
		= frameBuffer[(tmpy+7) * 480 + tmpx+1]
		= frameBuffer[(tmpy+7) * 480 + tmpx+2]
		= frameBuffer[(tmpy+7) * 480 + tmpx+3]
		= frameBuffer[(tmpy+7) * 480 + tmpx+4]
		= frameBuffer[(tmpy+7) * 480 + tmpx+5]
		= frameBuffer[(tmpy+7) * 480 + tmpx+6]
		= frameBuffer[(tmpy+7) * 480 + tmpx+7]
		= frameBuffer[(tmpy+7) * 480 + tmpx+8]
		= frameBuffer[(tmpy+7) * 480 + tmpx+9]
		= frameBuffer[(tmpy+7) * 480 + tmpx+10]
		= frameBuffer[(tmpy+7) * 480 + tmpx+11]
		= frameBuffer[(tmpy+7) * 480 + tmpx+12]
		= frameBuffer[(tmpy+7) * 480 + tmpx+13]
		= frameBuffer[(tmpy+7) * 480 + tmpx+14]
		= frameBuffer[(tmpy+7) * 480 + tmpx+15]

		= frameBuffer[(tmpy+8) * 480 + tmpx]
		= frameBuffer[(tmpy+8) * 480 + tmpx+1]
		= frameBuffer[(tmpy+8) * 480 + tmpx+2]
		= frameBuffer[(tmpy+8) * 480 + tmpx+3]
		= frameBuffer[(tmpy+8) * 480 + tmpx+4]
		= frameBuffer[(tmpy+8) * 480 + tmpx+5]
		= frameBuffer[(tmpy+8) * 480 + tmpx+6]
		= frameBuffer[(tmpy+8) * 480 + tmpx+7]
		= frameBuffer[(tmpy+8) * 480 + tmpx+8]
		= frameBuffer[(tmpy+8) * 480 + tmpx+9]
		= frameBuffer[(tmpy+8) * 480 + tmpx+10]
		= frameBuffer[(tmpy+8) * 480 + tmpx+11]
		= frameBuffer[(tmpy+8) * 480 + tmpx+12]
		= frameBuffer[(tmpy+8) * 480 + tmpx+13]
		= frameBuffer[(tmpy+8) * 480 + tmpx+14]
		= frameBuffer[(tmpy+8) * 480 + tmpx+15]

		= frameBuffer[(tmpy+9) * 480 + tmpx]
		= frameBuffer[(tmpy+9) * 480 + tmpx+1]
		= frameBuffer[(tmpy+9) * 480 + tmpx+2]
		= frameBuffer[(tmpy+9) * 480 + tmpx+3]
		= frameBuffer[(tmpy+9) * 480 + tmpx+4]
		= frameBuffer[(tmpy+9) * 480 + tmpx+5]
		= frameBuffer[(tmpy+9) * 480 + tmpx+6]
		= frameBuffer[(tmpy+9) * 480 + tmpx+7]
		= frameBuffer[(tmpy+9) * 480 + tmpx+8]
		= frameBuffer[(tmpy+9) * 480 + tmpx+9]
		= frameBuffer[(tmpy+9) * 480 + tmpx+10]
		= frameBuffer[(tmpy+9) * 480 + tmpx+11]
		= frameBuffer[(tmpy+9) * 480 + tmpx+12]
		= frameBuffer[(tmpy+9) * 480 + tmpx+13]
		= frameBuffer[(tmpy+9) * 480 + tmpx+14]
		= frameBuffer[(tmpy+9) * 480 + tmpx+15]

		= frameBuffer[(tmpy+10) * 480 + tmpx]
		= frameBuffer[(tmpy+10) * 480 + tmpx+1]
		= frameBuffer[(tmpy+10) * 480 + tmpx+2]
		= frameBuffer[(tmpy+10) * 480 + tmpx+3]
		= frameBuffer[(tmpy+10) * 480 + tmpx+4]
		= frameBuffer[(tmpy+10) * 480 + tmpx+5]
		= frameBuffer[(tmpy+10) * 480 + tmpx+6]
		= frameBuffer[(tmpy+10) * 480 + tmpx+7]
		= frameBuffer[(tmpy+10) * 480 + tmpx+8]
		= frameBuffer[(tmpy+10) * 480 + tmpx+9]
		= frameBuffer[(tmpy+10) * 480 + tmpx+10]
		= frameBuffer[(tmpy+10) * 480 + tmpx+11]
		= frameBuffer[(tmpy+10) * 480 + tmpx+12]
		= frameBuffer[(tmpy+10) * 480 + tmpx+13]
		= frameBuffer[(tmpy+10) * 480 + tmpx+14]
		= frameBuffer[(tmpy+10) * 480 + tmpx+15]

		= frameBuffer[(tmpy+11) * 480 + tmpx]
		= frameBuffer[(tmpy+11) * 480 + tmpx+1]
		= frameBuffer[(tmpy+11) * 480 + tmpx+2]
		= frameBuffer[(tmpy+11) * 480 + tmpx+3]
		= frameBuffer[(tmpy+11) * 480 + tmpx+4]
		= frameBuffer[(tmpy+11) * 480 + tmpx+5]
		= frameBuffer[(tmpy+11) * 480 + tmpx+6]
		= frameBuffer[(tmpy+11) * 480 + tmpx+7]
		= frameBuffer[(tmpy+11) * 480 + tmpx+8]
		= frameBuffer[(tmpy+11) * 480 + tmpx+9]
		= frameBuffer[(tmpy+11) * 480 + tmpx+10]
		= frameBuffer[(tmpy+11) * 480 + tmpx+11]
		= frameBuffer[(tmpy+11) * 480 + tmpx+12]
		= frameBuffer[(tmpy+11) * 480 + tmpx+13]
		= frameBuffer[(tmpy+11) * 480 + tmpx+14]
		= frameBuffer[(tmpy+11) * 480 + tmpx+15]

		= frameBuffer[(tmpy+12) * 480 + tmpx]
		= frameBuffer[(tmpy+12) * 480 + tmpx+1]
		= frameBuffer[(tmpy+12) * 480 + tmpx+2]
		= frameBuffer[(tmpy+12) * 480 + tmpx+3]
		= frameBuffer[(tmpy+12) * 480 + tmpx+4]
		= frameBuffer[(tmpy+12) * 480 + tmpx+5]
		= frameBuffer[(tmpy+12) * 480 + tmpx+6]
		= frameBuffer[(tmpy+12) * 480 + tmpx+7]
		= frameBuffer[(tmpy+12) * 480 + tmpx+8]
		= frameBuffer[(tmpy+12) * 480 + tmpx+9]
		= frameBuffer[(tmpy+12) * 480 + tmpx+10]
		= frameBuffer[(tmpy+12) * 480 + tmpx+11]
		= frameBuffer[(tmpy+12) * 480 + tmpx+12]
		= frameBuffer[(tmpy+12) * 480 + tmpx+13]
		= frameBuffer[(tmpy+12) * 480 + tmpx+14]
		= frameBuffer[(tmpy+12) * 480 + tmpx+15]

		= frameBuffer[(tmpy+13) * 480 + tmpx]
		= frameBuffer[(tmpy+13) * 480 + tmpx+1]
		= frameBuffer[(tmpy+13) * 480 + tmpx+2]
		= frameBuffer[(tmpy+13) * 480 + tmpx+3]
		= frameBuffer[(tmpy+13) * 480 + tmpx+4]
		= frameBuffer[(tmpy+13) * 480 + tmpx+5]
		= frameBuffer[(tmpy+13) * 480 + tmpx+6]
		= frameBuffer[(tmpy+13) * 480 + tmpx+7]
		= frameBuffer[(tmpy+13) * 480 + tmpx+8]
		= frameBuffer[(tmpy+13) * 480 + tmpx+9]
		= frameBuffer[(tmpy+13) * 480 + tmpx+10]
		= frameBuffer[(tmpy+13) * 480 + tmpx+11]
		= frameBuffer[(tmpy+13) * 480 + tmpx+12]
		= frameBuffer[(tmpy+13) * 480 + tmpx+13]
		= frameBuffer[(tmpy+13) * 480 + tmpx+14]
		= frameBuffer[(tmpy+13) * 480 + tmpx+15]

		= frameBuffer[(tmpy+14) * 480 + tmpx]
		= frameBuffer[(tmpy+14) * 480 + tmpx+1]
		= frameBuffer[(tmpy+14) * 480 + tmpx+2]
		= frameBuffer[(tmpy+14) * 480 + tmpx+3]
		= frameBuffer[(tmpy+14) * 480 + tmpx+4]
		= frameBuffer[(tmpy+14) * 480 + tmpx+5]
		= frameBuffer[(tmpy+14) * 480 + tmpx+6]
		= frameBuffer[(tmpy+14) * 480 + tmpx+7]
		= frameBuffer[(tmpy+14) * 480 + tmpx+8]
		= frameBuffer[(tmpy+14) * 480 + tmpx+9]
		= frameBuffer[(tmpy+14) * 480 + tmpx+10]
		= frameBuffer[(tmpy+14) * 480 + tmpx+11]
		= frameBuffer[(tmpy+14) * 480 + tmpx+12]
		= frameBuffer[(tmpy+14) * 480 + tmpx+13]
		= frameBuffer[(tmpy+14) * 480 + tmpx+14]
		= frameBuffer[(tmpy+14) * 480 + tmpx+15]

		= frameBuffer[(tmpy+15) * 480 + tmpx]
		= frameBuffer[(tmpy+15) * 480 + tmpx+1]
		= frameBuffer[(tmpy+15) * 480 + tmpx+2]
		= frameBuffer[(tmpy+15) * 480 + tmpx+3]
		= frameBuffer[(tmpy+15) * 480 + tmpx+4]
		= frameBuffer[(tmpy+15) * 480 + tmpx+5]
		= frameBuffer[(tmpy+15) * 480 + tmpx+6]
		= frameBuffer[(tmpy+15) * 480 + tmpx+7]
		= frameBuffer[(tmpy+15) * 480 + tmpx+8]
		= frameBuffer[(tmpy+15) * 480 + tmpx+9]
		= frameBuffer[(tmpy+15) * 480 + tmpx+10]
		= frameBuffer[(tmpy+15) * 480 + tmpx+11]
		= frameBuffer[(tmpy+15) * 480 + tmpx+12]
		= frameBuffer[(tmpy+15) * 480 + tmpx+13]
		= frameBuffer[(tmpy+15) * 480 + tmpx+14]
		= frameBuffer[(tmpy+15) * 480 + tmpx+15]
		=  tmpFrameBuffer;


	    }
	  }
	}
      }
    }
  }
  else if(zoomStyle == ZOOM){
    for(int i=0; i<dp.numRects(); i++){
      QRect r = dp.rect(i);

      int div = 1;
      if(canvasType == QVGA){
	div = 2;
      }

      for(int y=r.top(); y<=r.bottom(); y++){
	for(int x=r.left(); x<=r.right(); x++){
	  //lineStep()はcharで数えるため2で割る
	  //pixel()は24bitカラーを返すので16bitカラーへ変換


	  if(((y>>zoomScale)+zoomTop) >= SCREEN_HEIGHT/div 
	     || ((y>>zoomScale)+zoomTop) < 0
	     || ((x>>zoomScale)+zoomLeft)  >= SCREEN_WIDTH/div
	     || ((x>>zoomScale)+zoomLeft) < 0){

	    //	    bgImage->setPixel((x>>zoomScale), (y>>zoomScale), 0xff808080);
	    bgImage->setPixel(x, y, 0xff808080);

	    if(useColorMatrix){
	      frameBuffer[y * 480 + x] = 
		(( colorMatrix[0][((0xff808080 & 0x00ff0000)>>16) & 0x000000ff] << 8) & 0xf800 )
		| (( colorMatrix[1][((0xff808080 & 0x0000ff00)>>8) & 0x000000ff] << 3) & 0x07e0)
		| (( colorMatrix[2][((0xff808080 & 0x000000ff)) & 0x000000ff] >> 3) & 0x001f);
	    }
	    else{
	      frameBuffer[y * 480 + x] = 
		(( (0xff808080 & 0x00ff0000) >> 8) & 0xf800)
		| (((0xff808080 & 0x0000ff00) >> 5) & 0x07e0)
		| (((0xff808080 & 0x000000ff) >> 3) & 0x001f);
	    }

	  }
	  else{

	    if(layerVisible[0]){
	      tmpColor = layer[0]->pixel((x>>zoomScale)+zoomLeft,(y>>zoomScale)+zoomTop);
	    }
	    else{
	      if(x%20 < 10 && y%20 < 10){
		tmpColor = 0xffffffff;
	      }
	      else if(x%20 >= 10 && y%20 >= 10){
		tmpColor = 0xffffffff;
	      }
	      else{
		tmpColor = 0xffcccccc;
	      }
	    }

	    if(0 == currentlayer && bokeashiFlag && penPressFlag){
	      if(tool != ERASER && !kasanenuriFlag)
		tmpColor = calc(drawMode, tmpColor, *(drawChecker+((y>>zoomScale)+zoomTop)*SCREEN_WIDTH+(x>>zoomScale)+zoomLeft), penalpha2);
	      else
		tmpColor = drawPoint((x>>zoomScale) + zoomLeft,(y>>zoomScale) + zoomTop,*(drawChecker+((y>>zoomScale) + zoomTop)*SCREEN_WIDTH+(x>>zoomScale) + zoomLeft), -1, true);
	    }
	    for(int m=1; m<layernum; m++){
	      if(layerVisible[m]){
		int layerColor = layer[m]->pixel((x>>zoomScale)+zoomLeft,(y>>zoomScale)+zoomTop);
		if(m == currentlayer && bokeashiFlag && penPressFlag){
		  layerColor = drawPoint((x>>zoomScale) + zoomLeft,(y>>zoomScale) + zoomTop,*(drawChecker+((y>>zoomScale) + zoomTop)*SCREEN_WIDTH+(x>>zoomScale) + zoomLeft), -1, true);
		}
		tmpColor = calc(layerMode[m],tmpColor, layerColor, layerAlpha2[m]);
	      }
	    }

	    //	    bgImage->setPixel(zoomLeft+(x>>zoomScale), zoomTop+(y>>zoomScale), tmpColor);
	    bgImage->setPixel(x, y, tmpColor);


	    if(useColorMatrix){
	      frameBuffer[y * 480 + x] = 
		(( colorMatrix[0][((tmpColor & 0x00ff0000)>>16) & 0x000000ff] << 8) & 0xf800 )
		| (( colorMatrix[1][((tmpColor & 0x0000ff00)>>8) & 0x000000ff] << 3) & 0x07e0)
		| (( colorMatrix[2][((tmpColor & 0x000000ff)) & 0x000000ff] >> 3) & 0x001f);
	    }
	    else{
	      frameBuffer[y * 480 + x] = 
		(( (tmpColor & 0x00ff0000) >> 8) & 0xf800)
		| (((tmpColor & 0x0000ff00) >> 5) & 0x07e0)
		| (((tmpColor & 0x000000ff) >> 3) & 0x001f);
	    }

	  }
	}
      }
    }
  }


  if(dispLupeFlag){
    dispLupeFlag = false;
    lupePanel->show();
    toolbar->redrawToolBar();

  }
}

void Canvas::repaintRect()
{
  //  QRect off_rect(0,0,0,0);
  // QRectは使いづらい。QRect::setLeftでwidth()の値が変ってしまうため
  int myleft = 0;
  int mytop = 0;
  int mywidth = 0;
  int myheight = 0;

  short toolsize = 0;

  switch(tool){
  case PEN:
    toolsize = pensize;
    break;
  case ERASER:
    toolsize = erasersize;
    break;
  case BLUR:
    toolsize = blursize;
    break;
  case SHARPEN:
    toolsize = sharpensize;
    break;
  }



  // 再描画領域の指定	
  /*  X座標−増、  Y座標−増 */
  if( (prevpoint->x() < point->x()) && ( prevpoint->y() < point->y())){

    myleft = (prevpoint->x() - (toolsize/2+1));
    mytop = (prevpoint->y() - (toolsize/2+1));
    mywidth = (point->x() - prevpoint->x() + 2 +toolsize);
    myheight = (point->y() - prevpoint->y() + 2 +toolsize);
  }
  /*  X座標−増、  Y座標−減 */	
  else if (( prevpoint->x() <point->x()) && ( prevpoint->y() > point->y()) ){
    myleft = (prevpoint->x() - (toolsize/2+1));	
    mytop = (point->y()- (toolsize/2+1));
    mywidth = (point->x() - prevpoint->x() + 2 + toolsize);
    myheight = (prevpoint->y() - point->y() + 2 + toolsize);		
  }
  /*  X座標−減、  Y座標−増 */			
  else if(( prevpoint->x() > point->x()) && (prevpoint->y() <point->y())){
    myleft = (point->x() - (toolsize/2+1));
    mytop = (prevpoint->y() - (toolsize/2+1));
    mywidth = (prevpoint->x() - point->x() + 2 + toolsize);
    myheight = (point->y() - prevpoint->y() + 2 + toolsize);
  }
  /*  X座標−減、  Y座標−減 */		
  else if((prevpoint->x() > point->x()) && ( prevpoint->y() > point->y())){

    myleft = (point->x() - (toolsize/2+1));
    mytop = (point->y() - (toolsize/2+1));			
    mywidth = (prevpoint->x() - point->x() + 2 + toolsize);
    myheight = (prevpoint->y() - point->y() + 2 + toolsize);
  }
  /*  X座標−変わらず、  Y座標−増 */			
  else if(( prevpoint->x() == point->x()) && (prevpoint->y() <point->y())){
    myleft = (prevpoint->x() - (toolsize/2+1));	
    mytop = (prevpoint->y() - (toolsize/2+1));		
    mywidth = (2 + toolsize);
    myheight = (point->y() - prevpoint->y() + 2 + toolsize);
  }	
  /*  X座標−変わらず、  Y座標−減 */				
  else if(( prevpoint->x() == point->x()) && (prevpoint->y() >point->y())){
    myleft = (prevpoint->x() - (toolsize/2+1));
    mytop = (point->y() - (toolsize/2+1));		
    mywidth = (2 + toolsize);
    myheight = (prevpoint->y() - point->y() + 2 + toolsize);
  }
  /*  X座標−増、  Y座標−変わらず */			
  else if(( prevpoint->x() < point->x()) && (prevpoint->y() == point->y())){
    myleft = (prevpoint->x() - (toolsize/2+1));
    mytop = (prevpoint->y() - (toolsize/2+1));		
    mywidth = (point->x() - prevpoint->x() + 2 + toolsize);
    myheight = (2 + toolsize);
  }
  /*  X座標−減、  Y座標−変わらず */				
  else if(( prevpoint->x() > point->x()) && (prevpoint->y() == point->y())){
    myleft = (point->x() - (toolsize/2+1));
    mytop = (prevpoint->y() - (toolsize/2+1));
    mywidth = (prevpoint->x() - point->x() + 2 + toolsize);
    myheight = (2 + toolsize);
  }		
  /*  X座標−変わらず、  Y座標−変わらず */				
  else if( (prevpoint->x() == point->x()) && ( prevpoint->y() == point->y())){
    myleft = (point->x() - (toolsize/2+1));
    mytop = (point->y() - (toolsize/2+1));		
    mywidth = (2 + toolsize); 
    myheight = (2 + toolsize);		
  }


  if(myleft<0){
    mywidth = (mywidth - (-myleft));
    myleft = (0);
  }
  if(mytop<0){
    myheight = (myheight - (-mytop));
    mytop = (0);
  }

  if(myleft >= SCREEN_WIDTH){
    return;
  }
  if(myleft < 0){
    myleft = (0);
  }
  if(mytop >= SCREEN_HEIGHT){
    return;
  }	
  if(mytop < 0){
    mytop = (0);
  }
  if(mytop+myheight > SCREEN_HEIGHT){
    myheight = (SCREEN_HEIGHT-mytop);
  }
  if(myleft+mywidth > SCREEN_WIDTH){
    mywidth = (SCREEN_WIDTH-myleft);
  }
  if(myheight <= 1){
    myheight = (1);
  }
  if(mywidth <= 1 ){
    mywidth = (1);
  }

  if(undo_left == 480){
    undo_left = myleft;
    undo_right = myleft + mywidth -1;
  }
  if(undo_top == 640){
    undo_top = mytop;
    undo_bottom = mytop + myheight -1;
  }

  if(myleft < undo_left){
    undo_left = myleft;
  }
  if(myleft+mywidth-1 > undo_right){
    undo_right = myleft + mywidth -1;
  }
  if(mytop < undo_top){
    undo_top = mytop;
  }
  if(mytop+myheight-1 > undo_bottom){
    undo_bottom = mytop+myheight-1;
  }



  /*
  if(bokeashiFlag && kasanenuriFlag && penPressFlag){
    for(int y=undo_top; y<=undo_bottom; y++){
      for(int x=undo_left; x<=undo_right; x++){
	if(((*(drawChecker+y*SCREEN_WIDTH+x)>>24) & 0x000000ff) > 0){
	  drawPoint(x,y,*(drawChecker+y*SCREEN_WIDTH+x), -1, false);
	  *(drawChecker+y*SCREEN_WIDTH+x) = 0x00ffffff;
	}
      }
    }
  }
  */

  //  if(zoomStyle == ZOOM || canvasType == QVGA){
  if(zoomStyle == ZOOM){
    mywidth = (mywidth<<zoomScale);
    myheight = (myheight<<zoomScale);
    myleft = ((myleft - zoomLeft)<<zoomScale);
    mytop = ((mytop  - zoomTop)<<zoomScale);
  }
  else if(zoomStyle == NORMAL || zoomStyle == LUPE){
    myleft = ((myleft - canvasLeft));
    mytop = ((mytop  - canvasTop));
  }

  if(myleft<0){
    mywidth = (mywidth - (-myleft));
    myleft = (0);
  }
  if(mytop<0){
    myheight = (myheight - (-mytop));
    mytop = (0);
  }

  if(myleft >= SCREEN_WIDTH){
    return;
  }
  if(myleft < 0){
    myleft = (0);
  }
  if(mytop >= SCREEN_HEIGHT){
    return;
  }	
  if(mytop < 0){
    mytop = (0);
  }
  if(mytop+myheight > SCREEN_HEIGHT){
    myheight = (SCREEN_HEIGHT-mytop);
  }
  if(myleft+mywidth > SCREEN_WIDTH){
    mywidth = (SCREEN_WIDTH-myleft);
  }
  if(myheight <= 1){
    myheight = (1);
  }
  if(mywidth <= 1 ){
    mywidth = (1);
  }


  QRegion r;

  r = QRegion(myleft,mytop,mywidth,myheight,QRegion::Rectangle);
  repaint_left = myleft;
  repaint_top = mytop;
  repaint_width = mywidth;
  repaint_height = myheight;


  if(zoomStyle == LUPE){
    repaintRectFlag = true;
  }

  repaint(r, false);

  //    RGB2HSB();
}

void Canvas::mousePressEvent( QMouseEvent *e )
{
  if(windowDirection == YOKO){
    return;
  }

  if(!layerVisible[currentlayer]){
    return;
  }


  int div = 1;
  if(zoomStyle == LUPE && !spoitSelectFlag){
    if(canvasType == QVGA){
      div = 2;
    }
    if(lupeScale == 1){
      lupeLeft = canvasLeft + e->x()/div - LUPE_WIDTH/4;
      lupeTop = canvasTop + e->y()/div - LUPE_HEIGHT/4;
    }
    else if(lupeScale == 2){
      lupeLeft = canvasLeft + e->x()/div - LUPE_WIDTH/8;
      lupeTop = canvasTop + e->y()/div - LUPE_HEIGHT/8;
    }
    else if(lupeScale == 3){
      lupeLeft = canvasLeft + e->x()/div - LUPE_WIDTH/16;
      lupeTop = canvasTop + e->y()/div - LUPE_HEIGHT/16;
    }
    else if(lupeScale == 4){
      lupeLeft = canvasLeft + e->x()/div - LUPE_WIDTH/32;
      lupeTop = canvasTop + e->y()/div - LUPE_HEIGHT/32;
    }
    lupePanel->myReset();

    return;
  }

  if(zoomSelectFlag){
    return;
  }


  //  if(zoomStyle == ZOOM || canvasType == QVGA){
  if(zoomStyle == ZOOM){
    prevpoint2->setX((e->x()>>zoomScale));
    prevpoint2->setY((e->y()>>zoomScale));
    prevpoint->setX(zoomLeft+(e->x()>>zoomScale));
    prevpoint->setY(zoomTop+(e->y()>>zoomScale));
  }
  else if(zoomStyle == NORMAL || zoomStyle == LUPE){
    prevpoint2->setX((e->x()));
    prevpoint2->setY((e->y()));
    prevpoint->setX(canvasLeft+(e->x()));
    prevpoint->setY(canvasTop+(e->y()));

  }

  if(handFlag){
    return;
  }
  else if(moveRectFlag){
    return;
  }


  if(spoitSelectFlag){
    if(canvasType == VGA){
      if(prevpoint->x()<0 || prevpoint->x()>=480 || prevpoint->y() < 0 || prevpoint->y() >= 640)
      return;
    }
    else if(canvasType == QVGA && zoomStyle == ZOOM){
      if(prevpoint->x()<0 || prevpoint->x()>=240 || prevpoint->y() < 0 || prevpoint->y() >= 320)
      return;
    }

    toolbar->reverseSBCursor();

    if(canvasType == QVGA && zoomStyle == LUPE){
      //      pencolor = bgImage->pixel(e->x()/2,e->y()/2);
      pencolor = bgImage->pixel(e->x(),e->y());
    }
    else{
      pencolor = bgImage->pixel(e->x(),e->y());
    }
    
    //       bgImage->save("spoit.png","PNG");
    //    fprintf(stderr, "%d,%d:%d,%d\n",canvasLeft, canvasTop,prevpoint2->x(),prevpoint2->y());

    int tmphue = fgHu;
    RGB2HSB();
    int tmphue2 = fgHu;
    fgHu = tmphue;
    toolbar->reverseSBCursor();
    huebar->changeHueArea(COLORDIV-tmphue2, -1);
    toolbar->changeSBArea();
    toolbar->changeColorArea();

    //ツールバー画像の表示
    //    QPixmap toolbarPix = QPixmap();
    //    toolbarPix.convertFromImage(toolbar->toolbarImg, 0);
    //    toolbar->setBackgroundPixmap(toolbarPix);
    toolbar->repaint_left = SBAREA_X-2;
    toolbar->repaint_top = SBAREA_Y-2;
    toolbar->repaint_width = SBAREA_WIDTH+4;
    toolbar->repaint_height = SBAREA_HEIGHT+4;
    QRegion r(toolbar->repaint_left,toolbar->repaint_top,toolbar->repaint_width,toolbar->repaint_height,QRegion::Rectangle);
    toolbar->repaint(r, false);

    toolbar->repaint_left = COLORAREA_X;
    toolbar->repaint_top = COLORAREA_Y;
    toolbar->repaint_width = COLORAREA_WIDTH;
    toolbar->repaint_height = COLORAREA_HEIGHT;
    r =QRegion(toolbar->repaint_left,toolbar->repaint_top,toolbar->repaint_width,toolbar->repaint_height,QRegion::Rectangle);
    toolbar->repaint(r, false);


    toolbar->repaint_left = COLORAREA3_X;
    toolbar->repaint_top = COLORAREA3_Y;
    toolbar->repaint_width = COLORAREA3_WIDTH;
    toolbar->repaint_height = COLORAREA3_HEIGHT;
    r =QRegion(toolbar->repaint_left,toolbar->repaint_top,toolbar->repaint_width,toolbar->repaint_height,QRegion::Rectangle);
    toolbar->repaint(r, false);
    
    //    QPixmap huePix = QPixmap();
    //    huePix.convertFromImage(huebar->hueImg, 0);
    //    huebar->setBackgroundPixmap(huePix);
    
    repaint_left = 479-(int)(479*fgHu/COLORDIV)-1;
    if(repaint_left < 0){
      repaint_left = 0;
    }
    else if(repaint_left > 477){
      repaint_left = 477;
    }
    repaint_top = 0;
    repaint_width = 3;
    repaint_height = 20;
    toolbar->repaintColor(repaint_left, repaint_top, repaint_width, repaint_height, false);

    return;
  }


  fileChangeFlag = true;


  if(bokeashiFlag && undoFlag){
    for(int y=0; y<640; y++){
      for(int x=0; x<480; x++){
	*(drawChecker+x+y*480) = 0x00ffffff;
      }
    }
  }

  // Undo領域取得
  if(undo_top != 640 && undo_left != 480){
    for(int y=undo_top; y<=undo_bottom; y++){
      for(int x=undo_left; x<=undo_right; x++){
	*(undoBuffer+x+y*480) = layer[currentlayer]->pixel(x,y);
      }
    }
    undo_left = 480;
    undo_top = 640;
    undo_right = 480;
    undo_bottom = 640;
  }


  penDownFlag = true;
  penPressFlag = true;



}

void Canvas::mouseReleaseEvent( QMouseEvent *e )
{
  if(windowDirection == YOKO){
    return;
  }

  if(!layerVisible[currentlayer]){
    return;
  }

  int div = 1;
  if(zoomStyle == LUPE && !spoitSelectFlag){
    if(canvasType == QVGA){
      div = 2;
    }
    if(lupeScale == 1){
      lupeLeft = canvasLeft + e->x()/div - LUPE_WIDTH/4;
      lupeTop = canvasTop + e->y()/div - LUPE_HEIGHT/4;
    }
    else if(lupeScale == 2){
      lupeLeft = canvasLeft + e->x()/div - LUPE_WIDTH/8;
      lupeTop = canvasTop + e->y()/div - LUPE_HEIGHT/8;
    }
    else if(lupeScale == 3){
      lupeLeft = canvasLeft + e->x()/div - LUPE_WIDTH/16;
      lupeTop = canvasTop + e->y()/div - LUPE_HEIGHT/16;
    }
    else if(lupeScale == 4){
      lupeLeft = canvasLeft + e->x()/div - LUPE_WIDTH/32;
      lupeTop = canvasTop + e->y()/div - LUPE_HEIGHT/32;
    }
    lupePanel->myReset();
    return;
  }
  //  if(zoomStyle == LUPE){
  //    return;
  //  }

  if(zoomSelectFlag){
    lupePanel->hide();

    zoomSelectFlag = false;

    zoomStyle = ZOOM;

    if(canvasType == VGA && zoomScale == 0){
      zoomScale = lastScale;

      zoomLeft = canvasLeft+e->x() - (SCREEN_WIDTH>>(zoomScale+1));
      zoomTop = canvasTop+e->y() - (SCREEN_HEIGHT>>(zoomScale+1));

      if(zoomLeft < -(SCREEN_WIDTH>>(zoomScale+1)))
      	zoomLeft = -(SCREEN_WIDTH>>(zoomScale+1));
      if(zoomLeft > SCREEN_WIDTH - (SCREEN_WIDTH>>(zoomScale+1)))
	zoomLeft = SCREEN_WIDTH - (SCREEN_WIDTH>>(zoomScale+1));


      if(zoomTop < -(SCREEN_HEIGHT>>(zoomScale+1)))
	zoomTop = -(SCREEN_HEIGHT>>(zoomScale+1));
      if(zoomTop > SCREEN_HEIGHT - (SCREEN_HEIGHT>>(zoomScale+1)))
      	zoomTop = SCREEN_HEIGHT - (SCREEN_HEIGHT>>(zoomScale+1));

    }
    else if(canvasType == QVGA && zoomScale == 1){
      zoomScale = lastScale;
      zoomTop = zoomTop + (e->y()/2) - (SCREEN_HEIGHT >>(zoomScale+1));
      zoomLeft = zoomLeft + (e->x()/2) - (SCREEN_WIDTH >>(zoomScale+1));

      if(zoomLeft < -(SCREEN_WIDTH>>(zoomScale+1)))
      	zoomLeft = -(SCREEN_WIDTH>>(zoomScale+1));
      if(zoomLeft > SCREEN_WIDTH - (SCREEN_WIDTH>>(zoomScale+1)))
	zoomLeft = SCREEN_WIDTH - (SCREEN_WIDTH>>(zoomScale+1));

      if(zoomTop < -(SCREEN_HEIGHT>>(zoomScale+1)))
	zoomTop = -(SCREEN_HEIGHT>>(zoomScale+1));
      if(zoomTop > SCREEN_HEIGHT - (SCREEN_HEIGHT>>(zoomScale+1)))
      	zoomTop = SCREEN_HEIGHT - (SCREEN_HEIGHT>>(zoomScale+1));

    }

    toolbar->show();
    huebar->show();
    
    if(canvasType == VGA){
      scalePanel->changeScale(zoomScale, 0);
    }
    else if(canvasType == QVGA){
      if(zoomScale >=2){
	scalePanel->changeScale(zoomScale-1, 0);
      }
      else{
	scalePanel->changeScale(zoomScale, 0);
      }
    }

    if(!scalePanel->stickyFlag && scalePanel->isVisible()){
      scalePanel->hide();
      scalePanel->display = false;
    }

    for(int y=SCALEICON_Y; y<SCALEICON_Y+SCALEICON_HEIGHT; y++){
      for(int x=SCALEICON_X; x<SCALEICON_X+SCALEICON_WIDTH; x++){
	toolbar->toolbarImg.setPixel(x,y,toolbar->toolbar_normalImg.pixel(x,y));
      }
    }
    QPixmap toolbarPix = QPixmap();
    toolbarPix.convertFromImage(toolbar->toolbarImg, 0);



  QPainter p2;
  p2.begin( &toolbarPix );
  p2.setFont(QFont("lcfont", 8));
  p2.setPen( black );
  QString  tmpStr2 = "0";
  tmpStr2 = tmpStr2.sprintf( "%d", currentlayer+1);
  p2.drawText(QRect(6,6, 9 ,10),Qt::AlignCenter , tmpStr2);
  if(canvasType == VGA){
    tmpStr2 = tmpStr2.sprintf( "%d", layernum);
  }
  else if(canvasType == QVGA){
    tmpStr2 = tmpStr2.sprintf( "%dq", layernum);
  }
  p2.drawText(QRect(17,16, 11 ,10),Qt::AlignCenter , tmpStr2);

  if(useColorMatrix){
    tmpStr2 = tmpStr2.sprintf( "ca");
  }
  else{
    tmpStr2 = tmpStr2.sprintf( "  ");
  }
  p2.drawText(QRect(MESHICON_X,MESHICON_Y + MESHICON_HEIGHT-1 , 9 ,10),Qt::AlignLeft , tmpStr2);

  p2.end();

    toolbar->setBackgroundPixmap(toolbarPix);

    repaint();
    toolbar->repaint();
    

    return;
  }


  //  if(zoomStyle == ZOOM || canvasType == QVGA){
  if(zoomStyle == ZOOM){
    point2->setX((e->x()>>zoomScale));
    point2->setY((e->y()>>zoomScale));
    point->setX(zoomLeft+(e->x()>>zoomScale));
    point->setY(zoomTop+(e->y()>>zoomScale));
  }
  else if(zoomStyle == NORMAL || zoomStyle == LUPE){
    point2->setX((e->x()));
    point2->setY((e->y()));
    point->setX(canvasLeft+(e->x()));
    point->setY(canvasTop+(e->y()));
  }


  if(spoitSelectFlag){
    return;
  }



  if(handFlag){
    if(zoomStyle == ZOOM){
      zoomLeft -= (point2->x()-prevpoint2->x());
      if(zoomLeft < -(SCREEN_WIDTH>>(zoomScale+1)))
      	zoomLeft = -(SCREEN_WIDTH>>(zoomScale+1));
      if(zoomLeft > SCREEN_WIDTH - (SCREEN_WIDTH>>(zoomScale+1)))
	zoomLeft = SCREEN_WIDTH - (SCREEN_WIDTH>>(zoomScale+1));

      zoomTop -= (point2->y()-prevpoint2->y());
      if(zoomTop < -(SCREEN_HEIGHT>>(zoomScale+1)))
	zoomTop = -(SCREEN_HEIGHT>>(zoomScale+1));
      if(zoomTop > SCREEN_HEIGHT - (SCREEN_HEIGHT>>(zoomScale+1)))
      	zoomTop = SCREEN_HEIGHT - (SCREEN_HEIGHT>>(zoomScale+1));

    }
    else if(zoomStyle == NORMAL){

      canvasLeft -= (point2->x()-prevpoint2->x());
      if(canvasLeft < -(SCREEN_WIDTH>>(zoomScale+1)))
      	canvasLeft = -(SCREEN_WIDTH>>(zoomScale+1));
      if(canvasLeft > SCREEN_WIDTH - (SCREEN_WIDTH>>(zoomScale+1)))
	canvasLeft = SCREEN_WIDTH - (SCREEN_WIDTH>>(zoomScale+1));

      canvasTop -= (point2->y()-prevpoint2->y());
      if(canvasTop < -(SCREEN_HEIGHT>>(zoomScale+1)))
	canvasTop = -(SCREEN_HEIGHT>>(zoomScale+1));
      if(canvasTop > SCREEN_HEIGHT - (SCREEN_HEIGHT>>(zoomScale+1)))
      	canvasTop = SCREEN_HEIGHT - (SCREEN_HEIGHT>>(zoomScale+1));

    }
    //    repaint();
    int repaint_left = 0;
    int repaint_top = 0;
    int repaint_width = SCREEN_WIDTH;
    int repaint_height = SCREEN_HEIGHT;
    QRegion r(repaint_left,repaint_top,repaint_width,repaint_height,QRegion::Rectangle);
    repaint(r, false);

    //    fprintf(stderr,"repaint\n");

    return;
  }
  else if(moveRectFlag){
    int moveX=0, moveY=0;

    if(zoomStyle == ZOOM){
      moveX = (point2->x()-prevpoint2->x());
      moveY = (point2->y()-prevpoint2->y());

    }
    else if(zoomStyle == NORMAL){
      moveX = (point2->x()-prevpoint2->x());
      moveY = (point2->y()-prevpoint2->y());

    }


    moveRectX = moveRectX + moveX;
    moveRectY = moveRectY + moveY;

    if(moveRectX >= 479){
      moveRectX = 479;
    }
    if(moveRectY >= 639){
      moveRectY = 639;
    }
    if(moveRectX <= -479){
      moveRectX = -479;
    }
    if(moveRectY <= -639){
      moveRectY = -639;
    }


    if(moveRectX<0 && moveRectY<0){
      for(int y=0; y<640+moveRectY; y++){
	for(int x=0; x<480+moveRectX; x++){
	  layer[currentlayer]->setPixel(x,y,*(undoBuffer+(x-moveRectX)+(y-moveRectY)*480));
	}
      }
      unsigned int tmpColor = 0;
      if(currentlayer == 0){
	tmpColor = 0xffffffff;
      }
      else{
	tmpColor = 0x00ffffff;
      }
      for(int y=640+moveRectY; y<640; y++){
	for(int x=0; x<480; x++){
	  layer[currentlayer]->setPixel(x,y,tmpColor);
	}
      }
      for(int y=0; y<640; y++){
	for(int x=480+moveRectX; x<480; x++){
	  layer[currentlayer]->setPixel(x,y,tmpColor);
	}
      }
    }
    else if(moveRectX >= 0 && moveRectY<0){
      for(int y=0; y<640+moveRectY; y++){
	for(int x=479; x>moveRectX; x--){
	  layer[currentlayer]->setPixel(x,y,*(undoBuffer+(x-moveRectX)+(y-moveRectY)*480));
	}
      }
      unsigned int tmpColor = 0;
      if(currentlayer == 0){
	tmpColor = 0xffffffff;
      }
      else{
	tmpColor = 0x00ffffff;
      }
      for(int y=640+moveRectY; y<640; y++){
	for(int x=0; x<480; x++){
	  layer[currentlayer]->setPixel(x,y,tmpColor);
	}
      }
      for(int y=0; y<640; y++){
	for(int x=0; x<=moveRectX; x++){
	  layer[currentlayer]->setPixel(x,y,tmpColor);
	}
      }
    }
    else if(moveRectX<0 && moveRectY >= 0){
      for(int y=639; y>moveRectY; y--){
	for(int x=0; x<480+moveRectX; x++){
	  layer[currentlayer]->setPixel(x,y,*(undoBuffer+(x-moveRectX)+(y-moveRectY)*480));
	}
      }
      unsigned int tmpColor = 0;
      if(currentlayer == 0){
	tmpColor = 0xffffffff;
      }
      else{
	tmpColor = 0x00ffffff;
      }
      for(int y=0; y<=moveRectY; y++){
	for(int x=0; x<480; x++){
	  layer[currentlayer]->setPixel(x,y,tmpColor);
	}
      }
      for(int y=0; y<640; y++){
	for(int x=480+moveRectX; x<480; x++){
	  layer[currentlayer]->setPixel(x,y,tmpColor);
	}
      }
    }
    else if(moveRectX >= 0 && moveRectY >= 0){
      for(int y=639; y>moveRectY; y--){
	for(int x=479; x>moveRectX; x--){
	  layer[currentlayer]->setPixel(x,y,*(undoBuffer+(x-moveRectX)+(y-moveRectY)*480));
	}
      }
      unsigned int tmpColor = 0;
      if(currentlayer == 0){
	tmpColor = 0xffffffff;
      }
      else{
	tmpColor = 0x00ffffff;
      }
      for(int y=0; y<=moveRectY; y++){
	for(int x=0; x<480; x++){
	  layer[currentlayer]->setPixel(x,y,tmpColor);
	}
      }
      for(int y=0; y<640; y++){
	for(int x=0; x<=moveRectX; x++){
	  layer[currentlayer]->setPixel(x,y,tmpColor);
	}
      }
    }


    repaint();

    return;
  }
  else if(fillFlag){
    int div = 1;
    if(canvasType == QVGA)
      div = 2;
    if(point->x() >= 0 && point->x() < SCREEN_WIDTH/div 
       && point->y() >= 0 && point->y() < SCREEN_HEIGHT/div ){
      fill(point->x(),point->y());
      repaint();

      if(undoFlag){
	undoFlag = false;
	toolbar->redrawToolBar();
      }

      undo_left = 0;
      undo_top = 0;
      undo_right = 479;
      undo_bottom = 639;
    }
  }
  else{
    drawLine(false, true);
    repaintRect();

    penPressFlag = false;

    if(bokeashiFlag){
      for(int y=undo_top; y<=undo_bottom; y++){
	for(int x=undo_left; x<=undo_right; x++){
	  if(((*(drawChecker+y*SCREEN_WIDTH+x)>>24) & 0x000000ff) > 0){
	    drawPoint(x,y,*(drawChecker+y*SCREEN_WIDTH+x), -1, false);
	  }
	}
      }
    }

    if(undoFlag){
      undoFlag = false;
      toolbar->redrawToolBar();
    }
    
  }


  for(int i=0; i<SCREEN_WIDTH*SCREEN_HEIGHT; i++){
    *(drawChecker+i) = 0x00ffffff;
  }


}


void Canvas::mouseMoveEvent( QMouseEvent *e )
{
  if(windowDirection == YOKO){
    return;
  }


  if(!layerVisible[currentlayer]){
    return;
  }

  int div = 1;
  if(zoomStyle == LUPE && !spoitSelectFlag){
    if(canvasType == QVGA){
      div = 2;
    }
    if(lupeScale == 1){
      lupeLeft = canvasLeft + e->x()/div - LUPE_WIDTH/4;
      lupeTop = canvasTop + e->y()/div - LUPE_HEIGHT/4;
    }
    else if(lupeScale == 2){
      lupeLeft = canvasLeft + e->x()/div - LUPE_WIDTH/8;
      lupeTop = canvasTop + e->y()/div - LUPE_HEIGHT/8;
    }
    else if(lupeScale == 3){
      lupeLeft = canvasLeft + e->x()/div - LUPE_WIDTH/16;
      lupeTop = canvasTop + e->y()/div - LUPE_HEIGHT/16;
    }
    else if(lupeScale == 4){
      lupeLeft = canvasLeft + e->x()/div - LUPE_WIDTH/32;
      lupeTop = canvasTop + e->y()/div - LUPE_HEIGHT/32;
    }
    lupePanel->myReset();
    return;
  }
  //  if(zoomStyle == LUPE){
  //    return;
  //  }

  if(zoomSelectFlag){
    return;
  }

  //  if(zoomStyle == ZOOM || canvasType == QVGA){
  if(zoomStyle == ZOOM){
    point2->setX((e->x()>>zoomScale));
    point2->setY((e->y()>>zoomScale));
    point->setX(zoomLeft+(e->x()>>zoomScale));
    point->setY(zoomTop+(e->y()>>zoomScale));
  }
  else if(zoomStyle == NORMAL || zoomStyle == LUPE){
    point2->setX((e->x()));
    point2->setY((e->y()));
    point->setX(canvasLeft+(e->x()));
    point->setY(canvasTop+(e->y()));
  }


  if(spoitSelectFlag){
    if(canvasType == VGA){
      if(point->x()<0 || point->x()>=480 || point->y() < 0 || point->y() >= 640)
	return;
    }
    else if(canvasType == QVGA && zoomStyle == ZOOM){
      if(point->x()<0 || point->x()>=240 || point->y() < 0 || point->y() >= 320)
      return;
    }

    toolbar->reverseSBCursor();

    if(canvasType == QVGA && zoomStyle == LUPE){
      //      pencolor = bgImage->pixel(e->x()/2,e->y()/2);
      pencolor = bgImage->pixel(e->x(),e->y());
    }
    else{
      pencolor = bgImage->pixel(e->x(),e->y());
    }

    //  fprintf(stderr, "%d,%d,%d,%d\n",0x000000ff & (pencolor >>24), 0x000000ff & (pencolor>>16), 0x000000ff & (pencolor>>8), 0x000000ff & (pencolor));

    int tmphue = fgHu;
    RGB2HSB();
    int tmphue2 = fgHu;
    fgHu = tmphue;
    toolbar->reverseSBCursor();
    huebar->changeHueArea(COLORDIV-tmphue2, -1);
    toolbar->changeSBArea();
    toolbar->changeColorArea();

    //ツールバー画像の表示
    //    QPixmap toolbarPix = QPixmap();
    //    toolbarPix.convertFromImage(toolbar->toolbarImg, 0);
    //    toolbar->setBackgroundPixmap(toolbarPix);
    toolbar->repaint_left = SBAREA_X-2;
    toolbar->repaint_top = SBAREA_Y-2;
    toolbar->repaint_width = SBAREA_WIDTH+4;
    toolbar->repaint_height = SBAREA_HEIGHT+4;
    QRegion r(toolbar->repaint_left,toolbar->repaint_top,toolbar->repaint_width,toolbar->repaint_height,QRegion::Rectangle);
    toolbar->repaint(r, false);

    toolbar->repaint_left = COLORAREA_X;
    toolbar->repaint_top = COLORAREA_Y;
    toolbar->repaint_width = COLORAREA_WIDTH;
    toolbar->repaint_height = COLORAREA_HEIGHT;
    r =QRegion(toolbar->repaint_left,toolbar->repaint_top,toolbar->repaint_width,toolbar->repaint_height,QRegion::Rectangle);
    toolbar->repaint(r, false);


    toolbar->repaint_left = COLORAREA3_X;
    toolbar->repaint_top = COLORAREA3_Y;
    toolbar->repaint_width = COLORAREA3_WIDTH;
    toolbar->repaint_height = COLORAREA3_HEIGHT;
    r =QRegion(toolbar->repaint_left,toolbar->repaint_top,toolbar->repaint_width,toolbar->repaint_height,QRegion::Rectangle);
    toolbar->repaint(r, false);


    //    QPixmap huePix = QPixmap();
    //    huePix.convertFromImage(huebar->hueImg, 0);
    //    huebar->setBackgroundPixmap(huePix);

    repaint_left = 479-(int)(479*fgHu/COLORDIV)-1;
    if(repaint_left < 0){
      repaint_left = 0;
    }
    else if(repaint_left > 477){
      repaint_left = 477;
    }
    repaint_top = 0;
    repaint_width = 3;
    repaint_height = 20;
    toolbar->repaintColor(repaint_left, repaint_top, repaint_width, repaint_height, false);

    return;
  }

  if(handFlag){
    if(zoomStyle == ZOOM){
      zoomLeft -= (point2->x()-prevpoint2->x());
      if(zoomLeft < -(SCREEN_WIDTH>>(zoomScale+1)))
      	zoomLeft = -(SCREEN_WIDTH>>(zoomScale+1));
      if(zoomLeft > SCREEN_WIDTH - (SCREEN_WIDTH>>(zoomScale+1)))
	zoomLeft = SCREEN_WIDTH - (SCREEN_WIDTH>>(zoomScale+1));

      zoomTop -= (point2->y()-prevpoint2->y());
      if(zoomTop < -(SCREEN_HEIGHT>>(zoomScale+1)))
	zoomTop = -(SCREEN_HEIGHT>>(zoomScale+1));
      if(zoomTop > SCREEN_HEIGHT - (SCREEN_HEIGHT>>(zoomScale+1)))
      	zoomTop = SCREEN_HEIGHT - (SCREEN_HEIGHT>>(zoomScale+1));

    }
    else if(zoomStyle == NORMAL){

      canvasLeft -= (point2->x()-prevpoint2->x());
      if(canvasLeft < -(SCREEN_WIDTH>>(zoomScale+1)))
      	canvasLeft = -(SCREEN_WIDTH>>(zoomScale+1));
      if(canvasLeft > SCREEN_WIDTH - (SCREEN_WIDTH>>(zoomScale+1)))
	canvasLeft = SCREEN_WIDTH - (SCREEN_WIDTH>>(zoomScale+1));

      canvasTop -= (point2->y()-prevpoint2->y());
      if(canvasTop < -(SCREEN_HEIGHT>>(zoomScale+1)))
	canvasTop = -(SCREEN_HEIGHT>>(zoomScale+1));
      if(canvasTop > SCREEN_HEIGHT - (SCREEN_HEIGHT>>(zoomScale+1)))
      	canvasTop = SCREEN_HEIGHT - (SCREEN_HEIGHT>>(zoomScale+1));

    }

    repaint();


    prevpoint2->setX(point2->x());
    prevpoint2->setY(point2->y());
    return;
  }
  else if(moveRectFlag){
    int moveX=0, moveY=0;

    if(zoomStyle == ZOOM){
      moveX = (point2->x()-prevpoint2->x());
      moveY = (point2->y()-prevpoint2->y());

    }
    else if(zoomStyle == NORMAL){
      moveX = (point2->x()-prevpoint2->x());
      moveY = (point2->y()-prevpoint2->y());
    }

    moveRectX = moveRectX + moveX;
    moveRectY = moveRectY + moveY;

    if(moveRectX >= 479){
      moveRectX = 479;
    }
    if(moveRectY >= 639){
      moveRectY = 639;
    }
    if(moveRectX <= -479){
      moveRectX = -479;
    }
    if(moveRectY <= -639){
      moveRectY = -639;
    }


    if(moveRectX<0 && moveRectY<0){
      for(int y=0; y<640+moveRectY; y++){
	for(int x=0; x<480+moveRectX; x++){
	  layer[currentlayer]->setPixel(x,y,*(undoBuffer+(x-moveRectX)+(y-moveRectY)*480));
	}
      }
      unsigned int tmpColor = 0;
      if(currentlayer == 0){
	tmpColor = 0xffffffff;
      }
      else{
	tmpColor = 0x00ffffff;
      }
      for(int y=640+moveRectY; y<640; y++){
	for(int x=0; x<480; x++){
	  layer[currentlayer]->setPixel(x,y,tmpColor);
	}
      }
      for(int y=0; y<640; y++){
	for(int x=480+moveRectX; x<480; x++){
	  layer[currentlayer]->setPixel(x,y,tmpColor);
	}
      }
    }
    else if(moveRectX >= 0 && moveRectY<0){
      for(int y=0; y<640+moveRectY; y++){
	for(int x=479; x>moveRectX; x--){
	  layer[currentlayer]->setPixel(x,y,*(undoBuffer+(x-moveRectX)+(y-moveRectY)*480));
	}
      }
      unsigned int tmpColor = 0;
      if(currentlayer == 0){
	tmpColor = 0xffffffff;
      }
      else{
	tmpColor = 0x00ffffff;
      }
      for(int y=640+moveRectY; y<640; y++){
	for(int x=0; x<480; x++){
	  layer[currentlayer]->setPixel(x,y,tmpColor);
	}
      }
      for(int y=0; y<640; y++){
	for(int x=0; x<=moveRectX; x++){
	  layer[currentlayer]->setPixel(x,y,tmpColor);
	}
      }
    }
    else if(moveRectX<0 && moveRectY >= 0){
      for(int y=639; y>moveRectY; y--){
	for(int x=0; x<480+moveRectX; x++){
	  layer[currentlayer]->setPixel(x,y,*(undoBuffer+(x-moveRectX)+(y-moveRectY)*480));
	}
      }
      unsigned int tmpColor = 0;
      if(currentlayer == 0){
	tmpColor = 0xffffffff;
      }
      else{
	tmpColor = 0x00ffffff;
      }
      for(int y=0; y<=moveRectY; y++){
	for(int x=0; x<480; x++){
	  layer[currentlayer]->setPixel(x,y,tmpColor);
	}
      }
      for(int y=0; y<640; y++){
	for(int x=480+moveRectX; x<480; x++){
	  layer[currentlayer]->setPixel(x,y,tmpColor);
	}
      }
    }
    else if(moveRectX >= 0 && moveRectY >= 0){
      for(int y=639; y>moveRectY; y--){
	for(int x=479; x>moveRectX; x--){
	  layer[currentlayer]->setPixel(x,y,*(undoBuffer+(x-moveRectX)+(y-moveRectY)*480));
	}
      }
      unsigned int tmpColor = 0;
      if(currentlayer == 0){
	tmpColor = 0xffffffff;
      }
      else{
	tmpColor = 0x00ffffff;
      }
      for(int y=0; y<=moveRectY; y++){
	for(int x=0; x<480; x++){
	  layer[currentlayer]->setPixel(x,y,tmpColor);
	}
      }
      for(int y=0; y<640; y++){
	for(int x=0; x<=moveRectX; x++){
	  layer[currentlayer]->setPixel(x,y,tmpColor);
	}
      }
    }


    prevpoint2->setX(point2->x());
    prevpoint2->setY(point2->y());

    repaint();

    return;
  }
  else if(fillFlag){
    // nop
  }
  else{
    if(penDownFlag){
      drawLine(true, false);
      penDownFlag = false;
    }
    else{
      drawLine(false, false);
    }
    repaintRect();
    
    prevpoint->setX(point->x());
    prevpoint->setY(point->y());
  }
}



int Canvas::drawPoint(int x, int y, unsigned int color, int brushAlpha, bool drawFlag)
{
  unsigned int tmpColor = 0;
  unsigned int tmpR;
  unsigned int tmpG;
  unsigned int tmpB;
  unsigned int tmpAlpha;

  // 既に塗られている色のアルファ (0-256)

  int alpha1 = (layer[currentlayer]->pixel(x,y) >> 24) & 0x000000ff;
  if(!layerDrawable[currentlayer] && alpha1 == 0)
    return 0;

  if(bokeashiFlag &&  brushAlpha != -1){
    alpha1 = ( *(drawChecker+y*SCREEN_WIDTH+x) >> 24) & 0x000000ff;
  }

  if(alpha1 == 255){
    alpha1 = 256;
  }


  // これから塗る色のアルファ (0-256)
  int alpha2 = 0;
  if(bokeashiFlag && !kasanenuriFlag && brushAlpha != -1){
      if(brushAlpha == 255)
	alpha2 = 256;
      else
	alpha2 = brushAlpha;
      color = 0xff000000 | color;
  }
  else if(bokeashiFlag && !kasanenuriFlag && brushAlpha == -1){
    int myAlpha = (color >> 24) & 0x000000ff;
    if(myAlpha == 255){
      myAlpha = 256;
    }

    if(tool != ERASER){
      alpha2 = (myAlpha * penalpha2)>>8;
    }
    else{
      alpha2 = ((myAlpha) * eraseralpha2)>>8;
    }
    color = 0xff000000 | color;
  }
  else if(bokeashiFlag && kasanenuriFlag && brushAlpha != -1){
      if(brushAlpha == 255)
	alpha2 = 256;
      else
	alpha2 = brushAlpha;

    if(tool != ERASER){
      alpha2 = (alpha2 * penalpha2)>>8;
    }
    else{
      alpha2 = (alpha2 * eraseralpha2)>>8;
    }
    color = 0xff000000 | color;
  }
  else if(bokeashiFlag && kasanenuriFlag && brushAlpha == -1){
    alpha2 = (color >> 24) & 0x000000ff;
    if(alpha2 == 255){
      alpha2 = 256;
    }
  }
  else if(tool == PEN){
    alpha2 = penalpha2;
  }
  else if(tool == BLUR){
    alpha1 = 0;
    alpha2 = (bluralpha2 * ((color >> 24) & 0x000000ff))>>8;
  }
  else if(tool == SHARPEN){
    alpha2 = sharpenalpha2;
  }

  if(currentlayer != 0 || (bokeashiFlag)){

    if(tool == ERASER){
      if(bokeashiFlag  && brushAlpha != -1){
	tmpColor = 255 - ((*(drawChecker+y*SCREEN_WIDTH+x)>>24) & 0x000000ff);
	tmpColor = tmpColor - ((tmpColor)*alpha2>>8);
	tmpColor = ((255-tmpColor)<<24) & 0xff000000;
      }
      else if(bokeashiFlag  && brushAlpha == -1){
	if(currentlayer != 0){
	  if(alpha2 == 256){
	    tmpAlpha = 0;
	  }
	  else{
	    tmpAlpha = ((layer[currentlayer]->pixel(x,y)>>24)&0x000000ff) - (((layer[currentlayer]->pixel(x,y)>>24)&0x000000ff)*alpha2>>8);
	  }
	  
	  if(tmpAlpha != 0){
	    tmpColor = ((tmpAlpha<<24)&0xff000000) | (layer[currentlayer]->pixel(x,y)&0x00ffffff);
	  }
	  else{
	    tmpColor = 0x00ffffff;
	  }	
	}
	else{
	  tmpR = ((layer[currentlayer]->pixel(x,y)>>16)&0x000000ff) -  ((((layer[currentlayer]->pixel(x,y)>>16)&0x000000ff) - (0x000000ff))*alpha2>>8);
	  tmpG = ((layer[currentlayer]->pixel(x,y)>>8)&0x000000ff) -  ((((layer[currentlayer]->pixel(x,y)>>8)&0x000000ff) - (0x000000ff))*alpha2>>8);
	  tmpB = ((layer[currentlayer]->pixel(x,y))&0x000000ff) -  ((((layer[currentlayer]->pixel(x,y))&0x000000ff) - (0x000000ff))*alpha2>>8);
	  tmpColor = 0xff000000 | ((tmpR<<16)&0x00ff0000) | ((tmpG<<8)&0x0000ff00) | ((tmpB)&0x000000ff);
	}
      }
      else{
	if(eraseralpha == 100){
	  tmpAlpha = 0;
	}
	else{
	  tmpAlpha = ((layer[currentlayer]->pixel(x,y)>>24)&0x000000ff) - (((layer[currentlayer]->pixel(x,y)>>24)&0x000000ff)*eraseralpha2>>8);
	}
	if(tmpAlpha != 0){
	  tmpColor = ((tmpAlpha<<24)&0xff000000) | (layer[currentlayer]->pixel(x,y)&0x00ffffff);
	}
	else{
	  tmpColor = 0x00ffffff;
	}
      }

    }
    else{
      if(!(bokeashiFlag && brushAlpha != -1) && 
	 alpha1 == 0 && 
	 (drawMode == SCREEN
	  || drawMode == ADD
	  || drawMode == LIGHTEN
	  || drawMode == HUE
	  || drawMode == SATURATION
	  || drawMode == BRIGHTNESS
	  || drawMode == COLOR
	  || drawMode == BURN
	  || drawMode == DODGE
	  )
	 ){
	tmpColor = 0x00ffffff;
      }
      else{
	//合成後のアルファ
	unsigned int newAlpha = 0;
	newAlpha = (((256-alpha1)*alpha2)>>8) + alpha1;
	if(newAlpha >= 256){
	  newAlpha = 256;
	}

	if(!layerDrawable[currentlayer] && !(bokeashiFlag && brushAlpha != -1)){
	  newAlpha = (layer[currentlayer]->pixel(x,y) >> 24) & 0x000000ff;;
	}


	//	if(tool == BLUR){
	//	  newAlpha = alpha3;
	//	}


	// 既に塗られている色の白地に対するみかけの色
	unsigned int lowerColor = 0;
	if(bokeashiFlag  && brushAlpha != -1){
	  lowerColor = calc(NORMAL, 0xffffffff, *(drawChecker+y*SCREEN_WIDTH+x), 256);
	}
	else{
	  lowerColor = calc(NORMAL, 0xffffffff, layer[currentlayer]->pixel(x,y), 256);
	}

	unsigned int viewColor = 0;
	// ViewColorの値は正しい
	if(bokeashiFlag  && brushAlpha != -1){
	  viewColor = calc(NORMAL, lowerColor, color, alpha2);
	}
	else{
	  viewColor = calc(drawMode, lowerColor, color, alpha2);
	}

	// 合成後のアルファによって見た目の色をもたらすための実際の色を逆演算する．
	int realR = 255;
	int realG = 255;
	int realB = 255;
	if(newAlpha != 0){
	  realR = ((viewColor >> 16)&0x000000ff) - ((255*(256-newAlpha))>>8);
	  if(realR <= 0){
	    realR = 0;
	  }
	  else{
	    realR = (realR<<8) / newAlpha;
	    if(realR >=255){
	      realR = 255;
	    }
	  }
	
	  realG = ((viewColor >> 8)&0x000000ff) - ((255*(256-newAlpha))>>8);
	  if(realG <= 0){
	    realG = 0;
	  }
	  else{
	    realG = (realG<<8) / newAlpha;
	    if(realG >=255){
	      realG = 255;
	    }
	  }


	  realB = ((viewColor)&0x000000ff) - ((255*(256-newAlpha))>>8);
	  if(realB <= 0){
	    realB = 0;
	  }
	  else{
	    realB = (realB<<8) / newAlpha;
	    if(realB >=255){
	      realB = 255;
	    }
	  }
	}

	if(newAlpha >= 255){
	  newAlpha = 255;
	}

	tmpColor = ((newAlpha << 24)&0xff000000) | ((realR << 16)&0x00ff0000) | ((realG << 8)&0x0000ff00)  | ((realB )&0x000000ff);
      }
    }

  }
  else{
    if(tool == ERASER){
      tmpR = ((layer[currentlayer]->pixel(x,y)>>16)&0x000000ff) -  ((((layer[currentlayer]->pixel(x,y)>>16)&0x000000ff) - ((color>>16)&0x000000ff))*eraseralpha2>>8);
      tmpG = ((layer[currentlayer]->pixel(x,y)>>8)&0x000000ff) -  ((((layer[currentlayer]->pixel(x,y)>>8)&0x000000ff) - ((color>>8)&0x000000ff))*eraseralpha2>>8);
      tmpB = ((layer[currentlayer]->pixel(x,y))&0x000000ff) -  ((((layer[currentlayer]->pixel(x,y))&0x000000ff) - ((color)&0x000000ff))*eraseralpha2>>8);
      tmpColor = 0xff000000 | ((tmpR<<16)&0x00ff0000) | ((tmpG<<8)&0x0000ff00) | ((tmpB)&0x000000ff);
    }
    else{
      if(tool == PEN){
	tmpColor = calc(drawMode, layer[currentlayer]->pixel(x,y), color, penalpha2);
      }
      else if(tool == BLUR){
	tmpColor = calc(drawMode, layer[currentlayer]->pixel(x,y), color, bluralpha2);
      }
      else if(tool == SHARPEN){
	tmpColor = calc(drawMode, layer[currentlayer]->pixel(x,y), color, sharpenalpha2);
      }

    }

  }

  if(drawFlag){
    return tmpColor;
  }
  else if(bokeashiFlag  && brushAlpha != -1){
    *(drawChecker+y*SCREEN_WIDTH+x) = tmpColor;
  }
  else{
    layer[currentlayer]->setPixel(x,y,tmpColor);
  }

  //ぼかしツールを強力にするため
  if(tool != BLUR){
    if(bokeashiFlag){
      //nop
    }
    else{
      *(drawChecker+y*SCREEN_WIDTH+x)=0;
    }
  }

  return 0;

}



void Canvas::meshPoint(int x, int y, int brushAlpha, int toolAlpha)
{
  int drawcolor = pencolor;
  int div = 1;


  if(bokeashiFlag){
    if(kasanenuriFlag){
      // nop
    }
    else{

    }
  }
  else{
    if(*(drawChecker+y*SCREEN_WIDTH+x) == 0)
      return;
  }

  if(canvasType == QVGA){
    div = 2;
  }


  if(tool == ERASER){
    drawcolor = 0xffffffff;
  }

  if(x<0){
    return;
  }
  else if(x>=SCREEN_WIDTH/div){
    return;
  }
  if(y<0){
    return;
  }
  else if(y>=SCREEN_HEIGHT/div){
    return;
  }



  if(tool == BLUR){
    unsigned int tmpAlpha = 0;
    unsigned int tmpAlpha2 = 0;
    unsigned int tmpR = 0;
    unsigned int tmpG = 0;
    unsigned int tmpB = 0;
    unsigned int tmpR2 = 0;
    unsigned int tmpG2 = 0;
    unsigned int tmpB2 = 0;
    unsigned int tmpR3 = 0;
    unsigned int tmpG3 = 0;
    unsigned int tmpB3 = 0;

    unsigned int tmpColor = 0;
    unsigned int tmpColor2 = 0;

    int divNumColor = 1;
    int divNumAlpha = 1;

    tmpAlpha = (layer[currentlayer]->pixel(x,y)>>24)&0x000000ff;

    tmpAlpha2 = tmpAlpha*divNumAlpha;

    if(tmpAlpha == 255){
      tmpAlpha = 256;
    }

    tmpR = (layer[currentlayer]->pixel(x,y)>>16)&0x000000ff;
    tmpG = (layer[currentlayer]->pixel(x,y)>>8)&0x000000ff;
    tmpB = (layer[currentlayer]->pixel(x,y))&0x000000ff;

    if(tmpAlpha == 0){
      divNumColor = 0;

      tmpR = 0x00000000;
      tmpG = 0x00000000;
      tmpB = 0x00000000;

    }

    tmpR2 = tmpR*divNumColor;
    tmpG2 = tmpG*divNumColor;
    tmpB2 = tmpB*divNumColor;


    if(x>=1){
      if(currentlayer == 0){
	tmpAlpha = 256;
      }
      else{
	tmpAlpha = (layer[currentlayer]->pixel(x-1,y)>>24)&0x000000ff;
      }
      tmpAlpha2 += tmpAlpha;

      tmpR = 0;
      tmpG = 0;
      tmpB = 0;

      divNumAlpha++;

      if(tmpAlpha > 0){
	divNumColor++;
	tmpR = (layer[currentlayer]->pixel(x-1,y)>>16)&0x000000ff;
	tmpG = (layer[currentlayer]->pixel(x-1,y)>>8)&0x000000ff;
	tmpB = (layer[currentlayer]->pixel(x-1,y))&0x000000ff;
      }
      else if(tmpAlpha == 0){
	tmpR = 0x00000000;
	tmpG = 0x00000000;
	tmpB = 0x00000000;
      }

      tmpR2 += tmpR;
      tmpG2 += tmpG;
      tmpB2 += tmpB;
    }

    if(x<=SCREEN_WIDTH/div-2){
      if(currentlayer == 0){
	tmpAlpha = 256;
      }
      else{
	tmpAlpha = (layer[currentlayer]->pixel(x+1,y)>>24)&0x000000ff;
      }
      tmpAlpha2 += tmpAlpha;


      tmpR = 0;
      tmpG = 0;
      tmpB = 0;

      divNumAlpha++;

      if(tmpAlpha > 0){
	divNumColor++;
	tmpR = (layer[currentlayer]->pixel(x+1,y)>>16)&0x000000ff;
	tmpG = (layer[currentlayer]->pixel(x+1,y)>>8)&0x000000ff;
	tmpB = (layer[currentlayer]->pixel(x+1,y))&0x000000ff;
      }
      else if(tmpAlpha == 0){
	tmpR = 0x00000000;
	tmpG = 0x00000000;
	tmpB = 0x00000000;
      }

      tmpR2 += tmpR;
      tmpG2 += tmpG;
      tmpB2 += tmpB;
    }

    if(y>=1){
      if(currentlayer == 0){
	tmpAlpha = 256;
      }
      else{
	tmpAlpha = (layer[currentlayer]->pixel(x,y-1)>>24)&0x000000ff;
      }
      tmpAlpha2 += tmpAlpha;



      tmpR = 0;
      tmpG = 0;
      tmpB = 0;
      divNumAlpha++;

      if(tmpAlpha > 0){
	divNumColor++;
	tmpR = (layer[currentlayer]->pixel(x,y-1)>>16)&0x000000ff;
	tmpG = (layer[currentlayer]->pixel(x,y-1)>>8)&0x000000ff;
	tmpB = (layer[currentlayer]->pixel(x,y-1))&0x000000ff;
      }
      else if(tmpAlpha == 0){
	tmpR = 0x00000000;
	tmpG = 0x00000000;
	tmpB = 0x00000000;
      }

      tmpR2 += tmpR;
      tmpG2 += tmpG;
      tmpB2 += tmpB;

    }

    if(y<=SCREEN_HEIGHT/div-2){
      if(currentlayer == 0){
	tmpAlpha = 256;
      }
      else{
	tmpAlpha = (layer[currentlayer]->pixel(x,y+1)>>24)&0x000000ff;
      }
      tmpAlpha2 += tmpAlpha;

      tmpR = 0;
      tmpG = 0;
      tmpB = 0;

      divNumAlpha++;

      if(tmpAlpha > 0){
	divNumColor++;
	tmpR = (layer[currentlayer]->pixel(x,y+1)>>16)&0x000000ff;
	tmpG = (layer[currentlayer]->pixel(x,y+1)>>8)&0x000000ff;
	tmpB = (layer[currentlayer]->pixel(x,y+1))&0x000000ff;
      }
      else if(tmpAlpha == 0){
	tmpR = 0x00000000;
	tmpG = 0x00000000;
	tmpB = 0x00000000;
      }

      tmpR2 += tmpR;
      tmpG2 += tmpG;
      tmpB2 += tmpB;
      
    }

    if(divNumColor != 0){
      tmpR2 /= divNumColor;
      tmpG2 /= divNumColor;
      tmpB2 /= divNumColor;
    }
    if(tmpAlpha2 != 0){
      tmpAlpha2 /= divNumAlpha;
    }

    if(tmpAlpha2 == 0){
      return;
    }

    int tmpBlurAlpha = bluralpha2;
    if(powerBlurFlag){    
      tmpBlurAlpha = 254;
      //      tmpBlurAlpha = 256 - bluralpha2;
    }

    tmpAlpha = (layer[currentlayer]->pixel(x,y)>>24)&0x000000ff;
    tmpR = (layer[currentlayer]->pixel(x,y)>>16)&0x000000ff;
    tmpG = (layer[currentlayer]->pixel(x,y)>>8)&0x000000ff;
    tmpB = (layer[currentlayer]->pixel(x,y))&0x000000ff;
    
    tmpAlpha = tmpAlpha - (((tmpAlpha - tmpAlpha2) * tmpBlurAlpha)>>8);
    tmpR = tmpR - (((tmpR - tmpR2) * tmpBlurAlpha)>>8);
    tmpG = tmpG - (((tmpG - tmpG2) * tmpBlurAlpha)>>8);
    tmpB = tmpB - (((tmpB - tmpB2) * tmpBlurAlpha)>>8);

    tmpColor = ((tmpAlpha<<24)&0xff000000) | ((tmpR<<16)&0x00ff0000) | ((tmpG<<8)&0x0000ff00) | (tmpB&0x000000ff);

    layer[currentlayer]->setPixel(x,y,tmpColor);

    if(powerBlurFlag){    
      if(x >= 1){
	tmpR3 = (layer[currentlayer]->pixel(x-1,y)>>16)&0x000000ff;
	tmpG3 = (layer[currentlayer]->pixel(x-1,y)>>8)&0x000000ff;
	tmpB3 = (layer[currentlayer]->pixel(x-1,y))&0x000000ff;
	tmpR3 = tmpR3 - (((tmpR3 - tmpR) * bluralpha2)>>8);
	tmpG3 = tmpG3 - (((tmpG3 - tmpG) * bluralpha2)>>8);
	tmpB3 = tmpB3 - (((tmpB3 - tmpB) * bluralpha2)>>8);
	tmpColor = ((tmpAlpha<<24)&0xff000000) | ((tmpR3<<16)&0x00ff0000) | ((tmpG3<<8)&0x0000ff00) | (tmpB3&0x000000ff);
	layer[currentlayer]->setPixel(x-1,y,tmpColor);
      }
      if(x <= SCREEN_WIDTH/div-2){
	tmpR3 = (layer[currentlayer]->pixel(x+1,y)>>16)&0x000000ff;
	tmpG3 = (layer[currentlayer]->pixel(x+1,y)>>8)&0x000000ff;
	tmpB3 = (layer[currentlayer]->pixel(x+1,y))&0x000000ff;
	tmpR3 = tmpR3 - (((tmpR3 - tmpR) * bluralpha2)>>8);
	tmpG3 = tmpG3 - (((tmpG3 - tmpG) * bluralpha2)>>8);
	tmpB3 = tmpB3 - (((tmpB3 - tmpB) * bluralpha2)>>8);
	tmpColor = ((tmpAlpha<<24)&0xff000000) | ((tmpR3<<16)&0x00ff0000) | ((tmpG3<<8)&0x0000ff00) | (tmpB3&0x000000ff);
	layer[currentlayer]->setPixel(x+1,y,tmpColor);
      }
      if(y >= 1){
	tmpR3 = (layer[currentlayer]->pixel(x,y-1)>>16)&0x000000ff;
	tmpG3 = (layer[currentlayer]->pixel(x,y-1)>>8)&0x000000ff;
	tmpB3 = (layer[currentlayer]->pixel(x,y-1))&0x000000ff;
	tmpR3 = tmpR3 - (((tmpR3 - tmpR) * bluralpha2)>>8);
	tmpG3 = tmpG3 - (((tmpG3 - tmpG) * bluralpha2)>>8);
	tmpB3 = tmpB3 - (((tmpB3 - tmpB) * bluralpha2)>>8);
	tmpColor = ((tmpAlpha<<24)&0xff000000) | ((tmpR3<<16)&0x00ff0000) | ((tmpG3<<8)&0x0000ff00) | (tmpB3&0x000000ff);
	layer[currentlayer]->setPixel(x,y-1,tmpColor);
      }
      if(y <= SCREEN_HEIGHT/div-2){
	tmpR3 = (layer[currentlayer]->pixel(x,y+1)>>16)&0x000000ff;
	tmpG3 = (layer[currentlayer]->pixel(x,y+1)>>8)&0x000000ff;
	tmpB3 = (layer[currentlayer]->pixel(x,y+1))&0x000000ff;
	tmpR3 = tmpR3 - (((tmpR3 - tmpR) * bluralpha2)>>8);
	tmpG3 = tmpG3 - (((tmpG3 - tmpG) * bluralpha2)>>8);
	tmpB3 = tmpB3 - (((tmpB3 - tmpB) * bluralpha2)>>8);
	tmpColor = ((tmpAlpha<<24)&0xff000000) | ((tmpR3<<16)&0x00ff0000) | ((tmpG3<<8)&0x0000ff00) | (tmpB3&0x000000ff);
	layer[currentlayer]->setPixel(x,y+1,tmpColor);
      }

    }

    
    return;
  }
  else if(tool == SHARPEN){

    return;
  }


  if(meshpattern == MY_BLACK){
    drawPoint(x,y,drawcolor, brushAlpha, false);
  }
  else if(meshpattern == MY_DARK_GRAY){
    if( (y%2 == 0) 
	|| ((x%2 == 1) && (y%2 == 1))){	
      drawPoint(x,y, drawcolor, brushAlpha, false);
    }
    else if(overwrite == OVERWRITE && tool != ERASER){
      drawPoint(x,y, bgcolor, brushAlpha, false);
    }
    else{
      *(drawChecker+y*SCREEN_WIDTH+x)=0;
    }
  }
  else if(meshpattern == MY_GRAY){
    /* X,Y座標がともに奇数、あるいはともに偶数の時に描画 */
    if( ((x%2 == 0) && (y%2 == 0)) 
	|| ((x%2 == 1) && (y%2 == 1))){
      drawPoint(x,y, drawcolor, brushAlpha, false);
    }
    else if(overwrite == OVERWRITE && tool != ERASER){
      drawPoint(x,y, bgcolor, brushAlpha, false);
    }
    else{
      *(drawChecker+y*SCREEN_WIDTH+x)=0;
    }
  }
  else if(meshpattern == MY_FENCE_GRAY){
    if( ((x%2 == 0) && (y%2 == 0))
	|| ((x%4 == 1) && (y%4 == 1))
	|| ((x%4 == 3) && (y%4 == 3)) ) {
      drawPoint(x,y, drawcolor, brushAlpha, false);
    }
    else if(overwrite == OVERWRITE && tool != ERASER){
      drawPoint(x,y, bgcolor, brushAlpha, false);
    }
    else{
      *(drawChecker+y*SCREEN_WIDTH+x)=0;
    }
  }
  else if(meshpattern == MY_GRID_GRAY){
    /* X座標が偶数、かつY座標が偶数の時に描画 */
    if( (x%2 == 0) && (y%2 == 0)) {
      drawPoint(x,y, drawcolor, brushAlpha, false);
    }
    else if(overwrite == OVERWRITE && tool != ERASER){
      drawPoint(x,y, bgcolor, brushAlpha, false);
    }
    else{
      *(drawChecker+y*SCREEN_WIDTH+x)=0;
    }
  }
  else if(meshpattern == MY_LIGHT_GRAY){
    /* Y座標が4の倍数かつX座標が4の倍数の時、
       　　　あるいは、Y座標が4の倍数+2かつX座標が4の倍数+2の時に描画 */
    if( ((x%4 == 0) && (y%4 == 0))
	|| ((x%4 == 2) && (y%4 == 2))   ) {
      drawPoint(x,y, drawcolor, brushAlpha, false);
    }
    else if(overwrite == OVERWRITE && tool != ERASER){
      drawPoint(x,y, bgcolor, brushAlpha, false);
    }
    else{
      *(drawChecker+y*SCREEN_WIDTH+x)=0;
    }
  }	
  else if(meshpattern == MY_LEFT_INCLINE){
    if( x%4 == y%4 ){
      drawPoint(x,y, drawcolor, brushAlpha, false);
    }
    else if(overwrite == OVERWRITE && tool != ERASER){
      drawPoint(x,y, bgcolor, brushAlpha, false);
    }
    else{
      *(drawChecker+y*SCREEN_WIDTH+x)=0;
    }
  }	
  else if(meshpattern == MY_RIGHT_INCLINE){
    if( ((x%4 == 0) && (y%4 == 0))
	|| ((x%4 == 3) && (y%4 == 1))
	|| ((x%4 == 2) && (y%4 == 2))
	|| ((x%4 == 1) && (y%4 == 3)) ) {
      drawPoint(x,y, drawcolor, brushAlpha, false);
    }
    else if(overwrite == OVERWRITE && tool != ERASER){
      drawPoint(x,y, bgcolor, brushAlpha, false);
    }
    else{
      *(drawChecker+y*SCREEN_WIDTH+x)=0;
    }
  }	
  else if(meshpattern == MY_BORDER){
    if( y%2 == 0){
      drawPoint(x,y, drawcolor, brushAlpha, false);
    }
    else if(overwrite == OVERWRITE && tool != ERASER){
      drawPoint(x,y, bgcolor, brushAlpha, false);
    }
    else{
      *(drawChecker+y*SCREEN_WIDTH+x)=0;
    }
  }
  else if(meshpattern == MY_STRIPE){
    if( x%2 == 0){
      drawPoint(x,y, drawcolor, brushAlpha, false);
    }
    else if(overwrite == OVERWRITE && tool != ERASER){
      drawPoint(x,y, bgcolor, brushAlpha, false);
    }
    else{
      *(drawChecker+y*SCREEN_WIDTH+x)=0;
    }
  }	

}


void Canvas::scan_line(int lx,int rx,int y,unsigned int col )
{
  while ( lx <= rx ) {

    for ( ; lx < rx ; lx++ )
      if ( layer[currentlayer]->pixel( lx, y ) == col) break;
    if ( layer[currentlayer]->pixel( lx, y ) != col) break;


    for ( ; lx <= rx ; lx++ )
      if ( layer[currentlayer]->pixel( lx, y ) != col) break;
    End_Idx->sx = lx - 1;
    End_Idx->sy = y;
    if ( ++End_Idx == &Buffer[1024] ) End_Idx = Buffer;
  }
}


void Canvas::fill(int x, int y){
  int lx, rx, uy, dy;
  int i;
  unsigned int col;

  Start_Idx = Buffer;
  End_Idx = Buffer +1;
  Start_Idx->sx = x;
  Start_Idx->sy = y;
  col = layer[currentlayer]->pixel(x,y);

  do{
    lx = rx = Start_Idx->sx;
    uy = dy = Start_Idx->sy;
    if( ++Start_Idx == &Buffer[1024] )
      Start_Idx = Buffer;
    int div = 1;
    if(canvasType == QVGA){
      div = 2;
    }

    if (*(drawChecker+uy*SCREEN_WIDTH+lx) == 0 )
      continue;

    while (rx < SCREEN_WIDTH/div -1){
      if( layer[currentlayer]->pixel(rx+1,uy) != col) break;
      rx++;
    }

    while (lx > 0){
      if( layer[currentlayer]->pixel(lx-1,uy) != col) break;
      lx--;
    }
    for(i=lx; i<= rx; i++) meshPoint(i,uy, 256, 256);
    
    if( --uy >= 0)
      scan_line(lx,rx,uy,col);
    if( ++dy <= SCREEN_HEIGHT/div -1)
      scan_line(lx,rx,dy,col);
  }while (Start_Idx != End_Idx);


}

void Canvas::drawLine(bool penDown, bool penUp)
{

  int i,j;
  
  int x_width, y_width;
  int x_width2, y_width2;
  int x_hugou, y_hugou;

  int startx,starty,endx,endy;

  short toolsize = 0;

  int maxToolSize = 0;
  int minToolSize = 0;


  unsigned int tmpCol = pencolor;
  int alpha2 = 0;
  int tmpAlpha = 0;

  if(tool == PEN){
    alpha2 = penalpha2;
  }
  else if(tool == BLUR){
    alpha2 = bluralpha2;
  }
  else if(tool == SHARPEN){
    alpha2 = sharpenalpha2;
  }



  switch(tool){
  case PEN:
    toolsize = pensize;
    break;
  case ERASER:
    toolsize = erasersize;
    break;
  case BLUR:
    toolsize = blursize;
    break;
  case SHARPEN:
    toolsize = sharpensize;
    break;

  }

  startx = prevpoint->x();
  starty = prevpoint->y();
  endx = point->x();
  endy = point->y();

  x_width=endx - startx;
  y_width=endy - starty;

  x_width2 = x_width;
  y_width2 = y_width;


  int alphaA = 0;
  int alphaB = 0;
  int alphaC = 0;
  int alphaD = 0;
  if(bokeashiFlag){
    alphaA = 255;
    alphaB = 96;
    alphaC = 32;
    alphaD = 16;
  }


  if(x_width == 0){

    if(y_width < 0){
      y_hugou = -1;
    }
    else{
      y_hugou = 1;
    }


    if(toolsize <= 7){

      for(j=0; j<= y_width*y_hugou; j++){
	if(toolsize == 1){
	  meshPoint(startx,starty+j*y_hugou, alphaB, alpha2);
	}
	else if(toolsize == 2){
	  meshPoint(startx,starty+j*y_hugou, alphaB, alpha2);
	  meshPoint(startx+1,starty+j*y_hugou, alphaB, alpha2);
	  meshPoint(startx,starty+j*y_hugou+1, alphaB, alpha2);
	  meshPoint(startx+1,starty+j*y_hugou+1, alphaB, alpha2);
	}			
	else if(toolsize == 3){
	  meshPoint(startx,starty+j*y_hugou, alphaB, alpha2);
				
	  meshPoint(startx+1,starty+j*y_hugou, alphaC, alpha2);
	  meshPoint(startx-1,starty+j*y_hugou, alphaC, alpha2);
	  meshPoint(startx,starty+j*y_hugou+1, alphaC, alpha2);
	  meshPoint(startx,starty+j*y_hugou-1, alphaC, alpha2);

	}
	else if(toolsize == 4){
	  meshPoint(startx,starty+j*y_hugou, alphaB, alpha2);
	  meshPoint(startx-1,starty+j*y_hugou, alphaC, alpha2);
	  meshPoint(startx+1,starty+j*y_hugou, alphaB, alpha2);
	  meshPoint(startx+2,starty+j*y_hugou, alphaC, alpha2);

	  meshPoint(startx-1,starty+y_hugou*j+1, alphaC, alpha2);
	  meshPoint(startx,starty+y_hugou*j+1, alphaB, alpha2);
	  meshPoint(startx+1,starty+y_hugou*j+1, alphaB, alpha2);
	  meshPoint(startx+2,starty+y_hugou*j+1, alphaC, alpha2);

	  meshPoint(startx,starty+y_hugou*j-1, alphaC, alpha2);
	  meshPoint(startx+1,starty+y_hugou*j-1, alphaC, alpha2);

	  meshPoint(startx,starty+y_hugou*j+2, alphaC, alpha2);
	  meshPoint(startx+1,starty+y_hugou*j+2, alphaC, alpha2);
	}
	else if(toolsize == 5){
	  meshPoint(startx-2,starty+j*y_hugou, alphaC, alpha2);
	  meshPoint(startx-1,starty+j*y_hugou, alphaB, alpha2);
	  meshPoint(startx,starty+j*y_hugou, alphaB, alpha2);
	  meshPoint(startx+1,starty+j*y_hugou, alphaB, alpha2);
	  meshPoint(startx+2,starty+j*y_hugou, alphaC, alpha2);

	  meshPoint(startx-2,starty+y_hugou*j-1, alphaC, alpha2);
	  meshPoint(startx-1,starty+y_hugou*j-1, alphaB, alpha2);
	  meshPoint(startx,starty+y_hugou*j-1, alphaB, alpha2);
	  meshPoint(startx+1,starty+y_hugou*j-1, alphaB, alpha2);
	  meshPoint(startx+2,starty+y_hugou*j-1, alphaC, alpha2);

	  meshPoint(startx-2,starty+y_hugou*j+1, alphaC, alpha2);
	  meshPoint(startx-1,starty+y_hugou*j+1, alphaB, alpha2);
	  meshPoint(startx,starty+y_hugou*j+1, alphaB, alpha2);
	  meshPoint(startx+1,starty+y_hugou*j+1, alphaB, alpha2);
	  meshPoint(startx+2,starty+y_hugou*j+1, alphaC, alpha2);

	  meshPoint(startx-1,starty+y_hugou*j+2, alphaC, alpha2);
	  meshPoint(startx,starty+y_hugou*j+2, alphaC, alpha2);
	  meshPoint(startx+1,starty+y_hugou*j+2, alphaC, alpha2);

	  meshPoint(startx-1,starty+y_hugou*j-2, alphaC, alpha2);
	  meshPoint(startx,starty+y_hugou*j-2, alphaC, alpha2);
	  meshPoint(startx+1,starty+y_hugou*j-2, alphaC, alpha2);
	}
	else if(toolsize == 7){
	  meshPoint(startx-3,starty+j*y_hugou, alphaD, alpha2);
	  meshPoint(startx-2,starty+j*y_hugou, alphaC, alpha2);
	  meshPoint(startx-1,starty+j*y_hugou, alphaB, alpha2);
	  meshPoint(startx,starty+j*y_hugou, alphaB, alpha2);
	  meshPoint(startx+1,starty+j*y_hugou, alphaB, alpha2);
	  meshPoint(startx+2,starty+j*y_hugou, alphaC, alpha2);
	  meshPoint(startx+3,starty+j*y_hugou, alphaD, alpha2);

	  meshPoint(startx-3,starty+y_hugou*j-1, alphaD, alpha2);
	  meshPoint(startx-2,starty+y_hugou*j-1, alphaC, alpha2);
	  meshPoint(startx-1,starty+y_hugou*j-1, alphaB, alpha2);
	  meshPoint(startx,starty+y_hugou*j-1, alphaB, alpha2);
	  meshPoint(startx+1,starty+y_hugou*j-1, alphaB, alpha2);
	  meshPoint(startx+2,starty+y_hugou*j-1, alphaC, alpha2);
	  meshPoint(startx+3,starty+y_hugou*j-1, alphaD, alpha2);

	  meshPoint(startx-3,starty+y_hugou*j+1, alphaD, alpha2);
	  meshPoint(startx-2,starty+y_hugou*j+1, alphaC, alpha2);
	  meshPoint(startx-1,starty+y_hugou*j+1, alphaB, alpha2);
	  meshPoint(startx,starty+y_hugou*j+1, alphaB, alpha2);
	  meshPoint(startx+1,starty+y_hugou*j+1, alphaB, alpha2);
	  meshPoint(startx+2,starty+y_hugou*j+1, alphaC, alpha2);
	  meshPoint(startx+3,starty+y_hugou*j+1, alphaD, alpha2);

	  meshPoint(startx-2,starty+y_hugou*j+2, alphaD, alpha2);
	  meshPoint(startx-1,starty+y_hugou*j+2, alphaC, alpha2);
	  meshPoint(startx,starty+y_hugou*j+2, alphaC, alpha2);
	  meshPoint(startx+1,starty+y_hugou*j+2, alphaC, alpha2);
	  meshPoint(startx+2,starty+y_hugou*j+2, alphaD, alpha2);

	  meshPoint(startx-2,starty+y_hugou*j-2, alphaD, alpha2);
	  meshPoint(startx-1,starty+y_hugou*j-2, alphaC, alpha2);
	  meshPoint(startx,starty+y_hugou*j-2, alphaC, alpha2);
	  meshPoint(startx+1,starty+y_hugou*j-2, alphaC, alpha2);
	  meshPoint(startx+2,starty+y_hugou*j-2, alphaD, alpha2);

	  meshPoint(startx-1,starty+y_hugou*j+3, alphaD, alpha2);
	  meshPoint(startx,starty+y_hugou*j+3, alphaD, alpha2);
	  meshPoint(startx+1,starty+y_hugou*j+3, alphaD, alpha2);


	  meshPoint(startx-1,starty+y_hugou*j-3, alphaD, alpha2);
	  meshPoint(startx,starty+y_hugou*j-3, alphaD, alpha2);
	  meshPoint(startx+1,starty+y_hugou*j-3, alphaD, alpha2);


	}
      }
      return;
    }


    float r = toolsize/2;

    if(bokeashiFlag && penDown){
      for(int m=0; m<toolsize; m++){
	for(int n=0; n<toolsize; n++){
	  int alpha = 255 - (brushImage->pixel(m,n) & 0x000000ff);
	  alpha = (alpha * 196)>>8;
	  if(alpha == 0)
	    continue;
	  tmpAlpha = (alpha2 * alpha)>>8;
	  pencolor = (pencolor & 0x00ffffff) | ((tmpAlpha << 24) & 0xff000000);
	  meshPoint(startx-toolsize/2+m, starty-toolsize/2+n, alpha, alpha2);
	  pencolor = tmpCol;
	}
      }
    }
    else{
      if(!bokeashiFlag){
	for(int m=-r; m<=r; m++){
	  int tmpX = (sqrt((r)*(r) - m*m));
	  if(m==-r || m==r)
	    tmpX = 1;
	  if(m==-1 || m==1)
	    tmpX = r;

	  for(int n=-tmpX; n<=tmpX; n++){
	    int alpha = 0;
	    meshPoint(startx+n,starty+m, alpha, alpha2);
	  }
	}
      }
    }


    int counter = toolsize / 4 + 1 ;
    if(!bokeashiFlag)
      counter = 1;

    int startOffset = 0;
    int endOffset = 0;

    if(!bokeashiFlag || (bokeashiFlag && penDown))
      startOffset = 1;

    if(penUp)
      endOffset = -1;

    for(j=startOffset; j< y_width*y_hugou+endOffset; j+=counter){
      if(!bokeashiFlag){
	for(int n=-r; n<=r; n++){
	  meshPoint(startx+n,starty+j*y_hugou, 256, 256);
	}
      }
      else{
	for(int m=0; m<toolsize; m++){
	  for(int n=0; n<toolsize; n++){
	    int alpha = 255 - (brushImage->pixel(m,n) & 0x000000ff);
	  alpha = (alpha * 196)>>8;
	    if(alpha == 0)
	      continue;

	    tmpAlpha = (alpha2 * alpha)>>8;
	    pencolor = (pencolor & 0x00ffffff) | ((tmpAlpha << 24) & 0xff000000);
	    meshPoint(startx-toolsize/2+m, starty+y_hugou*j-toolsize/2+n, alpha, alpha2);
	    pencolor = tmpCol;
	  }
	}
      }
    }


    if(penUp){
      if(bokeashiFlag){
	for(int m=0; m<toolsize; m++){
	  for(int n=0; n<toolsize; n++){
	    int alpha = 255 - (brushImage->pixel(m,n) & 0x000000ff);
	  alpha = (alpha * 196)>>8;
	    if(alpha == 0)
	      continue;
	    tmpAlpha = (alpha2 * alpha)>>8;
	    pencolor = (pencolor & 0x00ffffff) | ((tmpAlpha << 24) & 0xff000000);
	    meshPoint(endx-toolsize/2+m, endy-toolsize/2+n, alpha, alpha2);
	    pencolor = tmpCol;
	  }
	}
      }
      else{
	for(int m=-r; m<=r; m++){
	  int tmpX = (sqrt((r)*(r) - m*m));
	  if(m==-r || m==r)
	    tmpX = 1;
	  if(m==-1 || m==1)
	    tmpX = r;
	  for(int n=-tmpX; n<=tmpX; n++){
	    int alpha = 0;
	    meshPoint(endx+n, endy+m, alpha, alpha2);
	    pencolor = tmpCol;
	  }
	}
      }
    }
    
  }
  else
    {
      if( (x_width > 0 && y_width >= 0) && (x_width > y_width)
	  || (x_width > 0 && y_width <= 0) && (x_width > -y_width)
	  || (x_width < 0 && y_width <= 0) && (-x_width > -y_width)
	  || (x_width < 0 && y_width >= 0) && (-x_width > y_width)
	  )
	{


	  if(x_width < 0){
	    x_width = -x_width;
	    x_hugou = -1;
	  }
	  else{
	    x_hugou = 1;
	  }

	  if(y_width < 0){
	    y_width = -y_width;
	    y_hugou = -1;
	  }
	  else{
	    y_hugou = 1;
	  }


	  if(toolsize <= 7){
	    for(j=0; j<=x_width-1; j++){
	      i = (int)((x_hugou*j*y_width2)/x_width2);

	      if(toolsize == 1){
		meshPoint(startx+x_hugou*j,starty+i, alphaB, alpha2);
	      }
	      else if(toolsize == 2){
		meshPoint(startx+x_hugou*j,starty+i, alphaB, alpha2);
		meshPoint(startx+x_hugou*j,starty+i+1, alphaB, alpha2);
		meshPoint(startx+x_hugou*j+1,starty+i+1, alphaB, alpha2);
		meshPoint(startx+x_hugou*j+1,starty+i, alphaB, alpha2);
	      }				
	      else if(toolsize == 3){
		meshPoint(startx+x_hugou*j,starty+i, alphaB, alpha2);
					
		meshPoint(startx+x_hugou*j+1,starty+i, alphaC, alpha2);
		meshPoint(startx+x_hugou*j-1,starty+i, alphaC, alpha2);					
		meshPoint(startx+x_hugou*j,starty+i+1, alphaC, alpha2);
		meshPoint(startx+x_hugou*j,starty+i-1, alphaC, alpha2);	

	      }
	      else if(toolsize == 4){
		meshPoint(startx+x_hugou*j,starty+i, alphaB, alpha2);
		meshPoint(startx+x_hugou*j,starty+i-1, alphaC, alpha2);	
		meshPoint(startx+x_hugou*j,starty+i+1, alphaB, alpha2);
		meshPoint(startx+x_hugou*j,starty+i+2, alphaC, alpha2);

		meshPoint(startx+x_hugou*j+1,starty+i-1, alphaC, alpha2);
		meshPoint(startx+x_hugou*j+1,starty+i, alphaB, alpha2);
		meshPoint(startx+x_hugou*j+1,starty+i+1, alphaB, alpha2);
		meshPoint(startx+x_hugou*j+1,starty+i+2, alphaC, alpha2);

		meshPoint(startx+x_hugou*j-1,starty+i, alphaC, alpha2);
		meshPoint(startx+x_hugou*j-1,starty+i+1, alphaC, alpha2);

		meshPoint(startx+x_hugou*j+2,starty+i, alphaC, alpha2);
		meshPoint(startx+x_hugou*j+2,starty+i+1, alphaC, alpha2);

	      }				
	      else if(toolsize == 5){
		meshPoint(startx+x_hugou*j,starty+i-2, alphaC, alpha2);	
		meshPoint(startx+x_hugou*j,starty+i-1, alphaB, alpha2);	
		meshPoint(startx+x_hugou*j,starty+i, alphaB, alpha2);
		meshPoint(startx+x_hugou*j,starty+i+1, alphaB, alpha2);
		meshPoint(startx+x_hugou*j,starty+i+2, alphaC, alpha2);

		meshPoint(startx+x_hugou*j-1,starty+i-2, alphaC, alpha2);
		meshPoint(startx+x_hugou*j-1,starty+i-1, alphaB, alpha2);
		meshPoint(startx+x_hugou*j-1,starty+i, alphaB, alpha2);
		meshPoint(startx+x_hugou*j-1,starty+i+1, alphaB, alpha2);
		meshPoint(startx+x_hugou*j-1,starty+i+2, alphaC, alpha2);

		meshPoint(startx+x_hugou*j+1,starty+i-2, alphaC, alpha2);
		meshPoint(startx+x_hugou*j+1,starty+i-1, alphaB, alpha2);
		meshPoint(startx+x_hugou*j+1,starty+i, alphaB, alpha2);
		meshPoint(startx+x_hugou*j+1,starty+i+1, alphaB, alpha2);
		meshPoint(startx+x_hugou*j+1,starty+i+2, alphaC, alpha2);

		meshPoint(startx+x_hugou*j+2,starty+i-1, alphaC, alpha2);
		meshPoint(startx+x_hugou*j+2,starty+i, alphaC, alpha2);
		meshPoint(startx+x_hugou*j+2,starty+i+1, alphaC, alpha2);

		meshPoint(startx+x_hugou*j-2,starty+i-1, alphaC, alpha2);
		meshPoint(startx+x_hugou*j-2,starty+i, alphaC, alpha2);
		meshPoint(startx+x_hugou*j-2,starty+i+1, alphaC, alpha2);
	      }				
	      else if(toolsize == 7){
		meshPoint(startx+x_hugou*j,starty+i-3, alphaD, alpha2);	
		meshPoint(startx+x_hugou*j,starty+i-2, alphaC, alpha2);	
		meshPoint(startx+x_hugou*j,starty+i-1, alphaB, alpha2);	
		meshPoint(startx+x_hugou*j,starty+i, alphaB, alpha2);
		meshPoint(startx+x_hugou*j,starty+i+1, alphaB, alpha2);
		meshPoint(startx+x_hugou*j,starty+i+2, alphaC, alpha2);
		meshPoint(startx+x_hugou*j,starty+i+3, alphaD, alpha2);

		meshPoint(startx+x_hugou*j-1,starty+i-3, alphaD, alpha2);
		meshPoint(startx+x_hugou*j-1,starty+i-2, alphaC, alpha2);
		meshPoint(startx+x_hugou*j-1,starty+i-1, alphaB, alpha2);
		meshPoint(startx+x_hugou*j-1,starty+i, alphaB, alpha2);
		meshPoint(startx+x_hugou*j-1,starty+i+1, alphaB, alpha2);
		meshPoint(startx+x_hugou*j-1,starty+i+2, alphaC, alpha2);
		meshPoint(startx+x_hugou*j-1,starty+i+3, alphaD, alpha2);

		meshPoint(startx+x_hugou*j+1,starty+i-3, alphaD, alpha2);
		meshPoint(startx+x_hugou*j+1,starty+i-2, alphaC, alpha2);
		meshPoint(startx+x_hugou*j+1,starty+i-1, alphaB, alpha2);
		meshPoint(startx+x_hugou*j+1,starty+i, alphaB, alpha2);
		meshPoint(startx+x_hugou*j+1,starty+i+1, alphaB, alpha2);
		meshPoint(startx+x_hugou*j+1,starty+i+2, alphaC, alpha2);
		meshPoint(startx+x_hugou*j+1,starty+i+3, alphaD, alpha2);

		meshPoint(startx+x_hugou*j+2,starty+i-2, alphaD, alpha2);
		meshPoint(startx+x_hugou*j+2,starty+i-1, alphaC, alpha2);
		meshPoint(startx+x_hugou*j+2,starty+i, alphaC, alpha2);
		meshPoint(startx+x_hugou*j+2,starty+i+1, alphaC, alpha2);
		meshPoint(startx+x_hugou*j+2,starty+i+2, alphaD, alpha2);

		meshPoint(startx+x_hugou*j-2,starty+i-2, alphaD, alpha2);
		meshPoint(startx+x_hugou*j-2,starty+i-1, alphaC, alpha2);
		meshPoint(startx+x_hugou*j-2,starty+i, alphaC, alpha2);
		meshPoint(startx+x_hugou*j-2,starty+i+1, alphaC, alpha2);
		meshPoint(startx+x_hugou*j-2,starty+i+2, alphaD, alpha2);

		meshPoint(startx+x_hugou*j+3,starty+i-1, alphaD, alpha2);
		meshPoint(startx+x_hugou*j+3,starty+i, alphaD, alpha2);
		meshPoint(startx+x_hugou*j+3,starty+i+1, alphaD, alpha2);

		meshPoint(startx+x_hugou*j-3,starty+i-1, alphaD, alpha2);
		meshPoint(startx+x_hugou*j-3,starty+i, alphaD, alpha2);
		meshPoint(startx+x_hugou*j-3,starty+i+1, alphaD, alpha2);

	      }				
	    }
	    return;
	  }



	  float r = toolsize / 2;

	  // y = x * w / h
	  // (1 + (w*w)/(h*h))* x*x = r*r
	  // x = r / sqrt(1 + (w*w)/(h*h))
	  // ---------------------------
	  // x = y * h / w
	  // (1 + (h*h)/(w*w)) * y*y = r*r
	  // y = r / sqrt(1 + (h*h)/(w*w))
	
	  float offY_f = r / sqrt(1 + (float)(y_width*y_width)/(float)(x_width*x_width));

	  //	float offY_f = offX_f * (float)y_width / (float)x_width;
	  //int offX = rint(offX_f);
	  int offY = rint(offY_f);

	  if(bokeashiFlag && penDown){
	    for(int m=0; m<toolsize; m++){
	      for(int n=0; n<toolsize; n++){
		int alpha = 255 - (brushImage->pixel(m,n) & 0x000000ff);
	  alpha = (alpha * 196)>>8;
		if(alpha == 0)
		  continue;

		tmpAlpha = (alpha2 * alpha)>>8;
		pencolor = (pencolor & 0x00ffffff) | ((tmpAlpha << 24) & 0xff000000);
		meshPoint(startx-toolsize/2+m, starty-toolsize/2+n, alpha, alpha2);
		pencolor = tmpCol;
	      }
	    }
	  }
	  else if(!bokeashiFlag){
	    for(int m=-r; m<=r; m++){
	      int tmpX = (sqrt((r)*(r) - m*m));
	      if(m==-r || m==r)
		tmpX = 1;
	      if(m==-1 || m==1)
		tmpX = r;
	      for(int n=-tmpX; n<=tmpX; n++){
		int alpha = 0;
		meshPoint(startx+n,starty+m, alpha, alpha2);
	      }
	    }
	  }

	  int startOffset = 0;
	  int endOffset = 0;

	  if(!bokeashiFlag || (bokeashiFlag && penDown))
	    startOffset = 1;
	  if(penUp)
	    endOffset = -1;

	  int counter = toolsize / 4 + 1 ;
	  if(!bokeashiFlag)
	    counter = 1;


	  for(j=startOffset; j<x_width+endOffset; j+=counter){
	    i = x_hugou*j*y_width2/x_width2;
	    if(!bokeashiFlag){
	      for(int n=-offY; n<=offY; n++){
		int m = x_hugou * (-y_hugou) * n *  y_width / x_width;
		meshPoint(startx+x_hugou*j+m,starty+i+n, 256, 256);
		if(n != -offY && n != offY){
		  meshPoint(startx+x_hugou*j+m-1,starty+i+n, 256, 256);
		  meshPoint(startx+x_hugou*j+m+1,starty+i+n, 256, 256);
		}
	      }
	    }
	    else{
	      for(int m=0; m<toolsize; m++){
		for(int n=0; n<toolsize; n++){
		  int alpha = 255 - (brushImage->pixel(m,n) & 0x000000ff);
	  alpha = (alpha * 196)>>8;
		  if(alpha == 0)
		    continue;
		  tmpAlpha = (alpha2 * alpha)>>8;
		  pencolor = (pencolor & 0x00ffffff) | ((tmpAlpha << 24) & 0xff000000);
		  meshPoint(startx+x_hugou*j-toolsize/2+m, starty+i-toolsize/2+n, alpha, alpha2);
		  pencolor = tmpCol;
		}
	      }
	    }
	  }



	  if(penUp){
	    if(bokeashiFlag){
	      for(int m=0; m<toolsize; m++){
		for(int n=0; n<toolsize; n++){
		  int alpha = 255 - (brushImage->pixel(m,n) & 0x000000ff);
	  alpha = (alpha * 196)>>8;
		  if(alpha == 0)
		    continue;
		  tmpAlpha = (alpha2 * alpha)>>8;
		  pencolor = (pencolor & 0x00ffffff) | ((tmpAlpha << 24) & 0xff000000);
		  meshPoint(endx-toolsize/2+m, endy-toolsize/2+n, alpha, alpha2);
		  pencolor = tmpCol;
		}
	      }
	    }
	    else{
	      for(int m=-r; m<=r; m++){
		int tmpX = (sqrt((r)*(r) - m*m));
		if(m==-r || m==r)
		  tmpX = 1;
		if(m==-1 || m==1)
		  tmpX = r;
		for(int n=-tmpX; n<=tmpX; n++){
		  int alpha = 0;
		  meshPoint(endx+n, endy+m, alpha, alpha2);
		}
	      }
	    }
	  }
	}

      else{
	if(y_width < 0){
	  y_width = -y_width;
	  y_hugou = -1;
	}
	else{
	  y_hugou = 1;
	}

	if(x_width < 0){
	  x_width = -x_width;
	  x_hugou = -1;
	}
	else{
	  x_hugou = 1;
	}


	if(toolsize <= 7){

	    for(i=0; i<=y_width-1; i++){
	      j = (int)((y_hugou*i*x_width2)/y_width2);

	      if(toolsize == 1){
		meshPoint(startx+j,starty+y_hugou*i, alphaB, alpha2);
	      }
	      else if(toolsize == 2){
		meshPoint(startx+j,starty+y_hugou*i, alphaB, alpha2);
		meshPoint(startx+j+1,starty+y_hugou*i, alphaB, alpha2);
		meshPoint(startx+j+1,starty+y_hugou*i+1, alphaB, alpha2);
		meshPoint(startx+j,starty+y_hugou*i+1, alphaB, alpha2);
	      }				
	      else if(toolsize == 3){
		meshPoint(startx+j,starty+y_hugou*i, alphaB, alpha2);

		meshPoint(startx+j+1,starty+y_hugou*i, alphaC, alpha2);
		meshPoint(startx+j-1,starty+y_hugou*i, alphaC, alpha2);	
		meshPoint(startx+j,starty+y_hugou*i+1, alphaC, alpha2);
		meshPoint(startx+j,starty+y_hugou*i-1, alphaC, alpha2);

	      }
	      else if(toolsize == 4){
		meshPoint(startx+j,starty+y_hugou*i, alphaB, alpha2);
		meshPoint(startx+j-1,starty+y_hugou*i, alphaC, alpha2);
		meshPoint(startx+j+1,starty+y_hugou*i, alphaB, alpha2);
		meshPoint(startx+j+2,starty+y_hugou*i, alphaC, alpha2);

		meshPoint(startx+j-1,starty+y_hugou*i+1, alphaC, alpha2);
		meshPoint(startx+j,starty+y_hugou*i+1, alphaB, alpha2);
		meshPoint(startx+j+1,starty+y_hugou*i+1, alphaB, alpha2);
		meshPoint(startx+j+2,starty+y_hugou*i+1, alphaC, alpha2);

		meshPoint(startx+j,starty+y_hugou*i-1, alphaC, alpha2);
		meshPoint(startx+j+1,starty+y_hugou*i-1, alphaC, alpha2);

		meshPoint(startx+j,starty+y_hugou*i+2, alphaC, alpha2);
		meshPoint(startx+j+1,starty+y_hugou*i+2, alphaC, alpha2);
	      }				
	      else if(toolsize == 5){
		meshPoint(startx+j-2,starty+y_hugou*i, alphaC, alpha2);
		meshPoint(startx+j-1,starty+y_hugou*i, alphaB, alpha2);
		meshPoint(startx+j,starty+y_hugou*i, alphaB, alpha2);
		meshPoint(startx+j+1,starty+y_hugou*i, alphaB, alpha2);
		meshPoint(startx+j+2,starty+y_hugou*i, alphaC, alpha2);

		meshPoint(startx+j-2,starty+y_hugou*i-1, alphaC, alpha2);
		meshPoint(startx+j-1,starty+y_hugou*i-1, alphaB, alpha2);
		meshPoint(startx+j,starty+y_hugou*i-1, alphaB, alpha2);
		meshPoint(startx+j+1,starty+y_hugou*i-1, alphaB, alpha2);
		meshPoint(startx+j+2,starty+y_hugou*i-1, alphaC, alpha2);

		meshPoint(startx+j-2,starty+y_hugou*i+1, alphaC, alpha2);
		meshPoint(startx+j-1,starty+y_hugou*i+1, alphaB, alpha2);
		meshPoint(startx+j,starty+y_hugou*i+1, alphaB, alpha2);
		meshPoint(startx+j+1,starty+y_hugou*i+1, alphaB, alpha2);
		meshPoint(startx+j+2,starty+y_hugou*i+1, alphaC, alpha2);

		meshPoint(startx+j-1,starty+y_hugou*i+2, alphaC, alpha2);
		meshPoint(startx+j,starty+y_hugou*i+2, alphaC, alpha2);
		meshPoint(startx+j+1,starty+y_hugou*i+2, alphaC, alpha2);

		meshPoint(startx+j-1,starty+y_hugou*i-2, alphaC, alpha2);
		meshPoint(startx+j,starty+y_hugou*i-2, alphaC, alpha2);
		meshPoint(startx+j+1,starty+y_hugou*i-2, alphaC, alpha2);
	      }				
	      else if(toolsize == 7){
		meshPoint(startx+j-3,starty+y_hugou*i, alphaD, alpha2);
		meshPoint(startx+j-2,starty+y_hugou*i, alphaC, alpha2);
		meshPoint(startx+j-1,starty+y_hugou*i, alphaB, alpha2);
		meshPoint(startx+j,starty+y_hugou*i, alphaB, alpha2);
		meshPoint(startx+j+1,starty+y_hugou*i, alphaB, alpha2);
		meshPoint(startx+j+2,starty+y_hugou*i, alphaC, alpha2);
		meshPoint(startx+j+3,starty+y_hugou*i, alphaD, alpha2);

		meshPoint(startx+j-3,starty+y_hugou*i-1, alphaD, alpha2);
		meshPoint(startx+j-2,starty+y_hugou*i-1, alphaC, alpha2);
		meshPoint(startx+j-1,starty+y_hugou*i-1, alphaB, alpha2);
		meshPoint(startx+j,starty+y_hugou*i-1, alphaB, alpha2);
		meshPoint(startx+j+1,starty+y_hugou*i-1, alphaB, alpha2);
		meshPoint(startx+j+2,starty+y_hugou*i-1, alphaC, alpha2);
		meshPoint(startx+j+3,starty+y_hugou*i-1, alphaD, alpha2);

		meshPoint(startx+j-3,starty+y_hugou*i+1, alphaD, alpha2);
		meshPoint(startx+j-2,starty+y_hugou*i+1, alphaC, alpha2);
		meshPoint(startx+j-1,starty+y_hugou*i+1, alphaB, alpha2);
		meshPoint(startx+j,starty+y_hugou*i+1, alphaB, alpha2);
		meshPoint(startx+j+1,starty+y_hugou*i+1, alphaB, alpha2);
		meshPoint(startx+j+2,starty+y_hugou*i+1, alphaC, alpha2);
		meshPoint(startx+j+3,starty+y_hugou*i+1, alphaD, alpha2);

		meshPoint(startx+j-2,starty+y_hugou*i+2, alphaD, alpha2);
		meshPoint(startx+j-1,starty+y_hugou*i+2, alphaC, alpha2);
		meshPoint(startx+j,starty+y_hugou*i+2, alphaC, alpha2);
		meshPoint(startx+j+1,starty+y_hugou*i+2, alphaC, alpha2);
		meshPoint(startx+j+2,starty+y_hugou*i+2, alphaD, alpha2);

		meshPoint(startx+j-2,starty+y_hugou*i-2, alphaD, alpha2);
		meshPoint(startx+j-1,starty+y_hugou*i-2, alphaC, alpha2);
		meshPoint(startx+j,starty+y_hugou*i-2, alphaC, alpha2);
		meshPoint(startx+j+1,starty+y_hugou*i-2, alphaC, alpha2);
		meshPoint(startx+j+2,starty+y_hugou*i-2, alphaD, alpha2);


		meshPoint(startx+j-1,starty+y_hugou*i+3, alphaD, alpha2);
		meshPoint(startx+j,starty+y_hugou*i+3, alphaD, alpha2);
		meshPoint(startx+j+1,starty+y_hugou*i+3, alphaD, alpha2);

		meshPoint(startx+j-1,starty+y_hugou*i-3, alphaD, alpha2);
		meshPoint(startx+j,starty+y_hugou*i-3, alphaD, alpha2);
		meshPoint(startx+j+1,starty+y_hugou*i-3, alphaD, alpha2);


	      }				
	    }
	    return;
	}



	float r = toolsize / 2;

	// y = x * w / h
	// (1 + (w*w)/(h*h))* x*x = r*r
	// x = r / sqrt(1 + (w*w)/(h*h))
	// ---------------------------
	// x = y * h / w
	// (1 + (h*h)/(w*w)) * y*y = r*r
	// y = r / sqrt(1 + (h*h)/(w*w))
	
	float offX_f = r / sqrt(1 + (float)(x_width*x_width)/(float)(y_width*y_width));

	//	float offY_f = offX_f * (float)y_width / (float)x_width;
	int offX = rint(offX_f);
	//int offY = rint(offY_f);

	if(bokeashiFlag && penDown){
	  for(int m=0; m<toolsize; m++){
	    for(int n=0; n<toolsize; n++){
	      int alpha = 255 - (brushImage->pixel(m,n) & 0x000000ff);
	  alpha = (alpha * 196)>>8;
	      if(alpha == 0)
		continue;

	      tmpAlpha = (alpha2 * alpha)>>8;
	      pencolor = (pencolor & 0x00ffffff) | ((tmpAlpha << 24) & 0xff000000);
	      meshPoint(startx-toolsize/2+m, starty-toolsize/2+n, alpha, alpha2);
	      pencolor = tmpCol;
	    }
	  }
	}
	else if(!bokeashiFlag){
	  for(int m=-r; m<=r; m++){
	    int tmpX = (sqrt((r)*(r) - m*m));
	    if(m==-r || m==r)
	      tmpX = 1;
	    if(m==-1 || m==1)
	      tmpX = r;
	    for(int n=-tmpX; n<=tmpX; n++){
	      int alpha = 0;
	      meshPoint(startx+n,starty+m, alpha, alpha2);
	    }
	  }
	}

	int startOffset = 0;
	int endOffset = 0;
	if(!bokeashiFlag || (bokeashiFlag && penDown))
	  startOffset = 1;
	if(penUp)
	  endOffset = -1;

	int counter = toolsize / 4 + 1;
	if(!bokeashiFlag)
	  counter = 1;

	for(i=startOffset; i<y_width+endOffset; i+=counter){
	  j = y_hugou*i*x_width2/y_width2;
	  if(!bokeashiFlag){
	    for(int m=-offX; m<=offX; m++){
	      int n = y_hugou * (-x_hugou) * m * x_width / y_width;

	      meshPoint(startx+j+m,starty+y_hugou*i+n, 256, 256);
	      if(m != -offX && m != offX){
		meshPoint(startx+j+m,starty+y_hugou*i+n-1, 256, 256);
		meshPoint(startx+j+m,starty+y_hugou*i+n+1, 256, 256);
	      }
	    }
	  }
	  else{

	    for(int m=0; m<toolsize; m++){
	      for(int n=0; n<toolsize; n++){
		int alpha = 255 - (brushImage->pixel(m,n) & 0x000000ff);
	  alpha = (alpha * 196)>>8;
		if(alpha == 0)
		  continue;
		tmpAlpha = (alpha2 * alpha)>>8;
		pencolor = (pencolor & 0x00ffffff) | ((tmpAlpha << 24) & 0xff000000);
		meshPoint(startx+j-toolsize/2+m, starty+y_hugou*i-toolsize/2+n, alpha, alpha2);
		pencolor = tmpCol;
	      }
	    }
	  }
	}
      

	if(penUp){
	  if(bokeashiFlag){
	    for(int m=0; m<toolsize; m++){
	      for(int n=0; n<toolsize; n++){
		int alpha = 255 - (brushImage->pixel(m,n) & 0x000000ff);
	  alpha = (alpha * 196)>>8;
		if(alpha == 0)
		  continue;

		tmpAlpha = (alpha2 * alpha)>>8;
		pencolor = (pencolor & 0x00ffffff) | ((tmpAlpha << 24) & 0xff000000);
		meshPoint(endx-toolsize/2+m, endy-toolsize/2+n, alpha, alpha2);
		pencolor = tmpCol;
	      }
	    }
	  }
	  else{
	    for(int m=-r; m<=r; m++){
	      int tmpX = (sqrt((r)*(r) - m*m));
	      if(m==-r || m==r)
		tmpX = 1;
	      if(m==-1 || m==1)
		tmpX = r;
	      for(int n=-tmpX; n<=tmpX; n++){
		int alpha = 0;
		meshPoint(endx+n, endy+m, alpha, alpha2);
	      }
	    }
	  }
	}

      }
    }
}

