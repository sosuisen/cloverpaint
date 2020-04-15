#include "clover.h"
#include "toolbarimage.h"
#include "toolbarimage_reverse.h"
#include "eraser_normal.h"
#include "eraser_reverse.h"
#include "eraser2_normal.h"
#include "eraser2_reverse.h"
#include "pencil_normal.h"
#include "pencil_reverse.h"
#include "brush_normal.h"
#include "brush_reverse.h"
#include "brush2_normal.h"
#include "brush2_reverse.h"
#include "blur_normal.h"
#include "blur_reverse.h"
#include "blur2_normal.h"
#include "blur2_reverse.h"
#include "sharpen_normal.h"
#include "sharpen_reverse.h"
#include "fill_normal.h"
#include "fill_reverse.h"

#include "hand.h"

#include "selectHide.h"

#include <math.h>

#define CHANGECOLORAREA_X 393
#define CHANGECOLORAREA_Y 2
#define CHANGECOLORAREA_WIDTH 33
#define CHANGECOLORAREA_HEIGHT 33

#define TOOLICON_L_X 31
#define TOOLICON_L_Y 2
#define TOOLICON_L_WIDTH 37
#define TOOLICON_L_HEIGHT 37

#define DRAWMODEICON_X 31
#define DRAWMODEICON_Y 39
#define DRAWMODEICON_WIDTH 37
#define DRAWMODEICON_HEIGHT 21



#define PENSIZEICON_X 68
#define PENSIZEICON_Y 2
#define PENSIZEICON_WIDTH 29
#define PENSIZEICON_HEIGHT 29

#define MESHICON_X 331
#define MESHICON_Y 2
#define MESHICON_WIDTH 29
#define MESHICON_HEIGHT 29

#define OPACITY_X 352
#define OPACITY_Y 47
#define OPACITY_WIDTH 100
#define OPACITY_HEIGHT 9

#define OPACITY_MINUS_X 333
#define OPACITY_MINUS_Y 39
#define OPACITY_MINUS_WIDTH 16
#define OPACITY_MINUS_HEIGHT 16

#define OPACITY_PLUS_X 459
#define OPACITY_PLUS_Y 39
#define OPACITY_PLUS_WIDTH 16
#define OPACITY_PLUS_HEIGHT 16


MyToolBar::MyToolBar(Canvas * parent, const char * name, bool modal, WFlags f)
  :QDialog(parent, name, modal, f)
{
  QPainter p;
  QString tmpStr = "   ";

  canvas = parent;

  reparent( parent, QPoint(0, canvas->toolbarY));
  resize(480, 62);


  opacitySelectingFlag = false;

  opacityMinusSelectFlag = false;
  opacityPlusSelectFlag = false;

  sbSelectingFlag = false;

  //ツールバー画像の読み込み
  QImage tmpImg = QImage(toolbarimage);
  //xpmからの読みこみはインデックスカラーになるため変換
  toolbarImg = tmpImg.convertDepth(32);
  toolbar_normalImg = toolbarImg.copy();

  //ツールバー画像(裏)の読み込み
  tmpImg = QImage(toolbarimage_reverse);
  toolbar_reverseImg = tmpImg.convertDepth(32);


  //消しゴム画像の読み込み
  tmpImg = QImage(eraser_normal);
  eraser_normalImg = tmpImg.convertDepth(32);
  tmpImg = QImage(eraser_reverse);
  eraser_reverseImg = tmpImg.convertDepth(32);

  tmpImg = QImage(eraser2_normal);
  eraser2_normalImg = tmpImg.convertDepth(32);
  tmpImg = QImage(eraser2_reverse);
  eraser2_reverseImg = tmpImg.convertDepth(32);

  //ペン画像の読み込み
  tmpImg = QImage(pencil_normal);
  pencil_normalImg = tmpImg.convertDepth(32);
  tmpImg = QImage(pencil_reverse);
  pencil_reverseImg = tmpImg.convertDepth(32);

  tmpImg = QImage(brush_normal);
  brush_normalImg = tmpImg.convertDepth(32);
  tmpImg = QImage(brush_reverse);
  brush_reverseImg = tmpImg.convertDepth(32);

  tmpImg = QImage(brush2_normal);
  brush2_normalImg = tmpImg.convertDepth(32);
  tmpImg = QImage(brush2_reverse);
  brush2_reverseImg = tmpImg.convertDepth(32);

  //ぼかし画像の読み込み
  tmpImg = QImage(blur_normal);
  blur_normalImg = tmpImg.convertDepth(32);
  tmpImg = QImage(blur_reverse);
  blur_reverseImg = tmpImg.convertDepth(32);
  tmpImg = QImage(blur2_normal);
  blur2_normalImg = tmpImg.convertDepth(32);
  tmpImg = QImage(blur2_reverse);
  blur2_reverseImg = tmpImg.convertDepth(32);

  //シャープ画像の読み込み
  tmpImg = QImage(sharpen_normal);
  sharpen_normalImg = tmpImg.convertDepth(32);
  tmpImg = QImage(sharpen_reverse);
  sharpen_reverseImg = tmpImg.convertDepth(32);

  tmpImg = QImage(fill_normal);
  fill_normalImg = tmpImg.convertDepth(32);
  tmpImg = QImage(fill_reverse);
  fill_reverseImg = tmpImg.convertDepth(32);


  //手のひら(選択不能)画像の読み込み
  tmpImg = QImage(hand);
  handImg = tmpImg.convertDepth(32);

  //選択ツール(選択不能)画像の読み込み
  tmpImg = QImage(selectHide);
  selectHideImg = tmpImg.convertDepth(32);


  for(int y=0; y<TOOLICON_L_HEIGHT; y++){
    for(int x=0; x<TOOLICON_L_WIDTH; x++){
      toolbarImg.setPixel(x+TOOLICON_L_X,y+TOOLICON_L_Y,pencil_reverseImg.pixel(x,y));
    }
  }

  //HUEバー作成用グラデーション描画
  //  canvas->fgSa = 169;
  //  canvas->fgBr = 57;
  //  for (int i=0; i<20; i++){
  //    for (int j=0; j<480; j++){	
  //      canvas->fgHu = j;
  //      canvas->HSB2RGB();
  //
  //      toolbarImg.setPixel(j,i, canvas->pencolor);
  //    }
  //  }


  canvas->fgHu = 0;
  for(int y=0; y<SBAREA_HEIGHT; y++){
    for(int x=0; x<SBAREA_WIDTH; x++){
      canvas->fgSa=x*COLORDIV/169;
      canvas->fgBr=(57-y)*COLORDIV/57;
      canvas->HSB2RGB();
      if(canvas->useColorMatrix){
	toolbarImg.setPixel(x+SBAREA_X,y+SBAREA_Y,
			    0xff000000 
			    | ((canvas->colorMatrix[0][((canvas->pencolor >> 16) & 0x000000ff)] << 16) & 0x00ff0000)
			    | ((canvas->colorMatrix[1][((canvas->pencolor >> 8) & 0x000000ff)] << 8) & 0x0000ff00)
			    | ((canvas->colorMatrix[2][((canvas->pencolor ) & 0x000000ff)] ) & 0x000000ff)
			    );
      }
      else{
	toolbarImg.setPixel(x+SBAREA_X,y+SBAREA_Y, canvas->pencolor);
      }
    }
  }

  canvas->pencolor=0xff000000;


  toolbarImg.setPixel(SBAREA_X+SBAREA_WIDTH-1-1,SBAREA_Y+SBAREA_HEIGHT-1, (~(toolbarImg.pixel(SBAREA_X+SBAREA_WIDTH-1-1,SBAREA_Y+SBAREA_HEIGHT-1)))|0xff000000 );
  toolbarImg.setPixel(SBAREA_X+SBAREA_WIDTH-1-2,SBAREA_Y+SBAREA_HEIGHT-1, (~(toolbarImg.pixel(SBAREA_X+SBAREA_WIDTH-1-2,SBAREA_Y+SBAREA_HEIGHT-1)))|0xff000000 );

  toolbarImg.setPixel(SBAREA_X+SBAREA_WIDTH-1,SBAREA_Y+SBAREA_HEIGHT-1-1, (~(toolbarImg.pixel(SBAREA_X+SBAREA_WIDTH-1,SBAREA_Y+SBAREA_HEIGHT-1-1)))|0xff000000);
  toolbarImg.setPixel(SBAREA_X+SBAREA_WIDTH-1,SBAREA_Y+SBAREA_HEIGHT-1-2, (~(toolbarImg.pixel(SBAREA_X+SBAREA_WIDTH-1,SBAREA_Y+SBAREA_HEIGHT-1-2)))|0xff000000);

  //  toolbarImg.setPixel(SBAREA_X+SBAREA_WIDTH-1+1,SBAREA_Y+SBAREA_HEIGHT-1, (~(toolbarImg.pixel(SBAREA_X+SBAREA_WIDTH-1+1,SBAREA_Y+SBAREA_HEIGHT-1)))|0xff000000);
  //  toolbarImg.setPixel(SBAREA_X+SBAREA_WIDTH-1+2,SBAREA_Y+SBAREA_HEIGHT-1, (~(toolbarImg.pixel(SBAREA_X+SBAREA_WIDTH-1+2,SBAREA_Y+SBAREA_HEIGHT-1)))|0xff000000);

  //toolbarImg.setPixel(SBAREA_X+SBAREA_WIDTH-1,SBAREA_Y+SBAREA_HEIGHT-1+1, (~(toolbarImg.pixel(SBAREA_X+SBAREA_WIDTH-1,SBAREA_Y+SBAREA_HEIGHT-1+1)))|0xff000000);
  //toolbarImg.setPixel(SBAREA_X+SBAREA_WIDTH-1,SBAREA_Y+SBAREA_HEIGHT-1+2, (~(toolbarImg.pixel(SBAREA_X+SBAREA_WIDTH-1,SBAREA_Y+SBAREA_HEIGHT-1+2)))|0xff000000);

  changeOpacityArea(OPACITY_X+OPACITY_WIDTH);

  //ツールバー画像の表示
  QPixmap toolbarPix = QPixmap();
  toolbarPix.convertFromImage(toolbarImg, 0);


  tmpStr = tmpStr.sprintf("%d",canvas->penalpha);

      p.begin( &toolbarPix );
      p.setPen( black );
      p.setFont(QFont("lcfont", 8));
      p.drawText(QRect(OPACITY_X,OPACITY_Y-11, OPACITY_WIDTH-7, 12),Qt::AlignRight , tmpStr);
      p.end();  

      toolbarImg = toolbarPix.convertToImage();


  QPainter p2;
  p2.begin( &toolbarPix );
  p2.setFont(QFont("lcfont", 8));
  p2.setPen( black );
  QString  tmpStr2 = "0";
  tmpStr2 = tmpStr2.sprintf( "%d", canvas->currentlayer+1);
  p2.drawText(QRect(6,6, 9 ,10),Qt::AlignCenter , tmpStr2);
  if(canvas->canvasType == VGA){
    tmpStr2 = tmpStr2.sprintf( "%d", canvas->layernum);
  }
  else if(canvas->canvasType == QVGA){
    tmpStr2 = tmpStr2.sprintf( "%dq", canvas->layernum);
  }
  p2.drawText(QRect(17,16, 11 ,10),Qt::AlignCenter , tmpStr2);

  if(canvas->useColorMatrix){
    tmpStr2 = tmpStr2.sprintf( "ca");
  }
  else{
    tmpStr2 = tmpStr2.sprintf( "  ");
  }
  p2.drawText(QRect(MESHICON_X,MESHICON_Y + MESHICON_HEIGHT-1 , 9 ,10),Qt::AlignLeft , tmpStr2);

  p2.end();


  repaint_left = 0;
  repaint_top = 0;
  repaint_width = 0;
  repaint_height = 0;


  setBackgroundPixmap(toolbarPix);



  repaint_left = 100;
  repaint_top = 0;
  repaint_width = 380;
  repaint_height = 62;

  changeSBArea();
  changeColorArea();

  repaint();




}

MyToolBar::~MyToolBar()
{

}


void MyToolBar::repaintLayerOpacity(int left, int top, int width, int height){

  repaint_left = left;
  repaint_top = top;
  repaint_width = width;
  repaint_height = height;
  QRegion r(repaint_left,repaint_top,repaint_width,repaint_height,QRegion::Rectangle);
  repaint(r, false);

}


void MyToolBar::repaintColor(int left, int top, int width, int height, bool repaintAll){
  repaint_left = left;
  repaint_top = top;
  repaint_width = width;
  repaint_height = height;
  QRegion r(repaint_left,repaint_top,repaint_width,repaint_height,QRegion::Rectangle);
  repaint(r, false);
  
 

  if(repaintAll){
  repaint_left = SBAREA_X-2;
  repaint_top = SBAREA_Y-2;
  repaint_width = SBAREA_WIDTH+4;
  repaint_height = SBAREA_HEIGHT+4;
  QRegion r(repaint_left,repaint_top,repaint_width,repaint_height,QRegion::Rectangle);
  repaint(r, false);

  repaint_left = COLORAREA_X;
  repaint_top = COLORAREA_Y;
  repaint_width = COLORAREA_WIDTH;
  repaint_height = COLORAREA_HEIGHT;
  r = QRegion(repaint_left,repaint_top,repaint_width,repaint_height,QRegion::Rectangle);
  repaint(r, false);

  repaint_left = COLORAREA3_X;
  repaint_top = COLORAREA3_Y;
  repaint_width = COLORAREA3_WIDTH;
  repaint_height = COLORAREA3_HEIGHT;
  r = QRegion(repaint_left,repaint_top,repaint_width,repaint_height,QRegion::Rectangle);
  repaint(r, false);
  }

}



void MyToolBar::paintEvent( QPaintEvent * )
{

  QDirectPainter dp(this);
  unsigned short *frameBuffer = (unsigned short *)dp.frameBuffer();

  if(repaint_width == 148){
    //layerPanelが対象のとき
    if(!canvas->layerPanel->isVisible()){
      return;
    }
    for(int y=repaint_top; y<repaint_top+repaint_height; y++){
      for(int x=repaint_left; x<repaint_left+repaint_width; x++){
	frameBuffer[(y+canvas->layerPanel->y()) * 480 + canvas->layerPanel->x()+x] = 
	  ((   (canvas->layerPanel->layerPanelImg->pixel(x,y) & 0x00ff0000) >> 8) & 0xf800)
	  | (((canvas->layerPanel->layerPanelImg->pixel(x,y) & 0x0000ff00) >> 5) & 0x07e0)
	  | (((canvas->layerPanel->layerPanelImg->pixel(x,y) & 0x000000ff) >> 3) & 0x001f);
      }

    }

  }
  else if(repaint_width == 3){
    // huebarが対象のとき
    for(int y=repaint_top; y<repaint_top+repaint_height; y++){
      for(int x=repaint_left; x<repaint_left+repaint_width; x++){
	if((y+canvas->huebarY<canvas->lupePanel->y() || y+canvas->huebarY>=canvas->lupePanel->y()+246 || x<canvas->lupePanel->x() || x>=canvas->lupePanel->x()+227  || !canvas->lupePanel->isVisible())
	   && ((y+canvas->huebarY<canvas->pensizePanel->y() || y+canvas->huebarY>=canvas->pensizePanel->y()+169 || x<canvas->pensizePanel->x() || x>=canvas->pensizePanel->x()+65) || !canvas->pensizePanel->isVisible())
	   && ((y+canvas->huebarY<canvas->meshPanel->y() || y+canvas->huebarY>=canvas->meshPanel->y()+201 || x<canvas->meshPanel->x() || x>=canvas->meshPanel->x()+65)  || !canvas->meshPanel->isVisible())
	   && ((y+canvas->huebarY<canvas->palettePanel->y() || y+canvas->huebarY>=canvas->palettePanel->y()+201 || x<canvas->palettePanel->x() || x>=canvas->palettePanel->x()+65)  || !canvas->palettePanel->isVisible())
	   && ((y+canvas->huebarY<canvas->scalePanel->y() || y+canvas->huebarY>=canvas->scalePanel->y()+111 || x<canvas->scalePanel->x() || x>=canvas->scalePanel->x()+65)  || !canvas->scalePanel->isVisible())
	   && ((y+canvas->huebarY<canvas->toolPanel->y() || y+canvas->huebarY>=canvas->toolPanel->y()+111 || x<canvas->toolPanel->x() || x>=canvas->toolPanel->x()+65)  || !canvas->toolPanel->isVisible())
	   && ((y+canvas->huebarY<canvas->layerPanel->y() || y+canvas->huebarY>=canvas->layerPanel->y()+canvas->layerPanel->height() || x<canvas->layerPanel->x() || x>=canvas->layerPanel->x()+canvas->layerPanel->width()) || !canvas->layerPanel->isVisible())
	   && ((y+canvas->huebarY<canvas->layerModePanel->y() || y+canvas->huebarY>=canvas->layerModePanel->y()+canvas->layerModePanel->height() || x<canvas->layerModePanel->x() || x>=canvas->layerModePanel->x()+canvas->layerModePanel->width()) || !canvas->layerModePanel->isVisible())
	   && ((y+canvas->huebarY<canvas->drawModePanel->y() || y+canvas->huebarY>=canvas->drawModePanel->y()+canvas->drawModePanel->height() || x<canvas->drawModePanel->x() || x>=canvas->drawModePanel->x()+canvas->drawModePanel->width()) || !canvas->drawModePanel->isVisible())
	   ){

		frameBuffer[(y+canvas->huebarY) * 480 + x] = 
		  ((   (canvas->huebar->hueImg.pixel(x,y) & 0x00ff0000) >> 8) & 0xf800)
		  | (((canvas->huebar->hueImg.pixel(x,y) & 0x0000ff00) >> 5) & 0x07e0)
		  | (((canvas->huebar->hueImg.pixel(x,y) & 0x000000ff) >> 3) & 0x001f);
	}
      }

    }


  }
  else{
    for(int y=repaint_top; y<repaint_top+repaint_height; y++){
      for(int x=repaint_left; x<repaint_left+repaint_width; x++){
	if((y+canvas->toolbarY<canvas->lupePanel->y() || y+canvas->toolbarY>=canvas->lupePanel->y()+246 || x<canvas->lupePanel->x() || x>=canvas->lupePanel->x()+227 || !canvas->lupePanel->isVisible())
	   && ((y+canvas->toolbarY<canvas->pensizePanel->y() || y+canvas->toolbarY>=canvas->pensizePanel->y()+169 || x<canvas->pensizePanel->x() || x>=canvas->pensizePanel->x()+65) || !canvas->pensizePanel->isVisible())
	   && ((y+canvas->toolbarY<canvas->meshPanel->y() || y+canvas->toolbarY>=canvas->meshPanel->y()+201 || x<canvas->meshPanel->x() || x>=canvas->meshPanel->x()+65)  || !canvas->meshPanel->isVisible())
	   && ((y+canvas->toolbarY<canvas->palettePanel->y() || y+canvas->toolbarY>=canvas->palettePanel->y()+201 || x<canvas->palettePanel->x() || x>=canvas->palettePanel->x()+65)  || !canvas->palettePanel->isVisible())
	   && ((y+canvas->toolbarY<canvas->scalePanel->y() || y+canvas->toolbarY>=canvas->scalePanel->y()+111 || x<canvas->scalePanel->x() || x>=canvas->scalePanel->x()+65)  || !canvas->scalePanel->isVisible())
	   && ((y+canvas->toolbarY<canvas->toolPanel->y() || y+canvas->toolbarY>=canvas->toolPanel->y()+111 || x<canvas->toolPanel->x() || x>=canvas->toolPanel->x()+65)  || !canvas->toolPanel->isVisible())
	   && ((y+canvas->toolbarY<canvas->layerPanel->y() || y+canvas->toolbarY>=canvas->layerPanel->y()+canvas->layerPanel->height() || x<canvas->layerPanel->x() || x>=canvas->layerPanel->x()+canvas->layerPanel->width()) || !canvas->layerPanel->isVisible())
	   && ((y+canvas->toolbarY<canvas->layerModePanel->y() || y+canvas->toolbarY>=canvas->layerModePanel->y()+276 || x<canvas->layerModePanel->x() || x>=canvas->layerModePanel->x()+65) || !canvas->layerModePanel->isVisible())
	   && ((y+canvas->toolbarY<canvas->drawModePanel->y() || y+canvas->toolbarY>=canvas->drawModePanel->y()+276 || x<canvas->drawModePanel->x() || x>=canvas->drawModePanel->x()+65) || !canvas->drawModePanel->isVisible())
	   ){
		frameBuffer[(y+canvas->toolbarY) * 480 + x] = 
		  (((toolbarImg.pixel(x,y) & 0x00ff0000) >> 8) & 0xf800)
		  | (((toolbarImg.pixel(x,y) & 0x0000ff00) >> 5) & 0x07e0)
		  | (((toolbarImg.pixel(x,y) & 0x000000ff) >> 3) & 0x001f);
	}
      }

    }

  }
}

void MyToolBar::redrawPenSize(int size){
  int left=0;
  int top=0;

  switch(size){
  case 1:
    left = 32;
    top = 20;	  
    break;
  case 2:
    left = 32;
    top = 49;
    break;
  case 3:
    left = 32;
    top = 78;	  
    break;
  case 4:
    left = 32;
    top = 107;
    break;
  case 5:
    left = 32;
    top = 136;	  
    break;
  case 7:
    left = 32;
    top = 165;	  
    break;
  case 11:
    left = 3;
    top = 20;	  
    break;
  case 17:
    left = 3;
    top = 49;
    break;
  case 25:
    left = 3;
    top = 78;	  
    break;
  case 33:
    left = 3;
    top = 107;	  
    break;
  case 65:
    left = 3;
    top = 136;	  
    break;
  case 99:
    left = 3;
    top = 165;	  
    break;
  default:
    break;
  }

  if(canvas->pensizePanel->isVisible()){
    for(int y=0; y<PENSIZEICON_HEIGHT; y++){
      for(int x=0; x<PENSIZEICON_WIDTH; x++){
	toolbarImg.setPixel(x+PENSIZEICON_X,y+PENSIZEICON_Y,canvas->pensize_reverseImg->pixel(x+left,y+top));
      }
    }
  }
  else{
    for(int y=0; y<PENSIZEICON_HEIGHT; y++){
      for(int x=0; x<PENSIZEICON_WIDTH; x++){
	toolbarImg.setPixel(x+PENSIZEICON_X,y+PENSIZEICON_Y,canvas->pensize_normalImg->pixel(x+left,y+top));
      }
    }
  }
    QPixmap toolbarPix = QPixmap();
    toolbarPix.convertFromImage(toolbarImg, 0);
  repaint_left = 0;
  repaint_top = 0;
  repaint_width = 0;
  repaint_height = 0;

    setBackgroundPixmap(toolbarPix);
    /*
  repaint_left = 0;
  repaint_top = 0;
  repaint_width = 480;
  repaint_height = 62;

  repaint();
    */
}

void MyToolBar::redrawMesh(){

  int left=0;
  int top=0;

  
  switch(canvas->meshpattern){
  case MY_BLACK:
    left = 32;
    top = 20;
    break;
  case MY_DARK_GRAY:
    left = 32;
    top = 49;
    break;
  case MY_GRAY:
    left = 32;
    top = 78;
    break;
  case MY_FENCE_GRAY:
    left = 32;
    top = 107;
    break;
  case MY_GRID_GRAY:
    left = 32;
    top = 136;
    break;
  case MY_LIGHT_GRAY:
    left = 3;
    top = 20;	  
    break;
  case MY_LEFT_INCLINE:
    left = 3;
    top = 49;
    break;
  case MY_RIGHT_INCLINE:
    left = 3;
    top = 78;
    break;
  case MY_BORDER:
    left = 3;
    top = 107;	  
    break;
  case MY_STRIPE:
    left = 3;
    top = 136;	  
    break;
  default:
    break;
  }

  if(canvas->meshPanel->isVisible()){
    for(int y=0; y<MESHICON_HEIGHT; y++){
      for(int x=0; x<MESHICON_WIDTH; x++){
	toolbarImg.setPixel(x+MESHICON_X,y+MESHICON_Y,canvas->mesh_reverseImg->pixel(x+left,y+top));
      }
    }
  }
  else{
    for(int y=0; y<MESHICON_HEIGHT; y++){
      for(int x=0; x<MESHICON_WIDTH; x++){
	toolbarImg.setPixel(x+MESHICON_X,y+MESHICON_Y,canvas->mesh_normalImg->pixel(x+left,y+top));
      }
    }
  }

    QPixmap toolbarPix = QPixmap();
    toolbarPix.convertFromImage(toolbarImg, 0);
  repaint_left = 0;
  repaint_top = 0;
  repaint_width = 0;
  repaint_height = 0;

    setBackgroundPixmap(toolbarPix);
    /*
  repaint_left = 0;
  repaint_top = 0;
  repaint_width = 480;
  repaint_height = 62;

  repaint();
    */
}



void MyToolBar::redrawToolBar()
{
  //ペンサイズ表示の再描画
  int toolsize=0;
  switch(canvas->tool){
  case PEN:
    toolsize=canvas->pensize;
    break;
  case ERASER:
    toolsize=canvas->erasersize;
    break;
  case BLUR:
    toolsize=canvas->blursize;
    break;
  case SHARPEN:
    toolsize=canvas->sharpensize;
    break;
  }
  redrawPenSize(toolsize);

  redrawMesh();

  changeColorArea();

  if(canvas->handFlag || (canvas->spoitSelectFlag && !canvas->spoitJogSelectFlag)){
    if(canvas->tool == PEN){
      if(canvas->fillFlag){
	for(int y=0; y<TOOLICON_L_HEIGHT; y++){
	  for(int x=0; x<TOOLICON_L_WIDTH; x++){
	    toolbarImg.setPixel(x+TOOLICON_L_X,y+TOOLICON_L_Y,fill_normalImg.pixel(x,y));
	  }
	}
      }
      else if(canvas->bokeashiFlag && canvas->kasanenuriFlag){
	for(int y=0; y<TOOLICON_L_HEIGHT; y++){
	  for(int x=0; x<TOOLICON_L_WIDTH; x++){
	    toolbarImg.setPixel(x+TOOLICON_L_X,y+TOOLICON_L_Y,brush2_normalImg.pixel(x,y));
	  }
	}
      }
      else if(canvas->bokeashiFlag && !canvas->kasanenuriFlag){
	for(int y=0; y<TOOLICON_L_HEIGHT; y++){
	  for(int x=0; x<TOOLICON_L_WIDTH; x++){
	    toolbarImg.setPixel(x+TOOLICON_L_X,y+TOOLICON_L_Y,brush_normalImg.pixel(x,y));
	  }
	}
      }
      else{
	for(int y=0; y<TOOLICON_L_HEIGHT; y++){
	  for(int x=0; x<TOOLICON_L_WIDTH; x++){
	    toolbarImg.setPixel(x+TOOLICON_L_X,y+TOOLICON_L_Y,pencil_normalImg.pixel(x,y));
	  }
	}
      }
    }
    else if(canvas->tool == ERASER){
      if(canvas->bokeashiFlag){
	for(int y=0; y<TOOLICON_L_HEIGHT; y++){
	  for(int x=0; x<TOOLICON_L_WIDTH; x++){
	    toolbarImg.setPixel(x+TOOLICON_L_X,y+TOOLICON_L_Y,eraser2_normalImg.pixel(x,y));
	  }
	}
      }
      else{
	for(int y=0; y<TOOLICON_L_HEIGHT; y++){
	  for(int x=0; x<TOOLICON_L_WIDTH; x++){
	    toolbarImg.setPixel(x+TOOLICON_L_X,y+TOOLICON_L_Y,eraser_normalImg.pixel(x,y));
	  }
	}

      }
    }
    else if(canvas->tool == BLUR){
      if(canvas->powerBlurFlag){    
	for(int y=0; y<TOOLICON_L_HEIGHT; y++){
	  for(int x=0; x<TOOLICON_L_WIDTH; x++){
	    toolbarImg.setPixel(x+TOOLICON_L_X,y+TOOLICON_L_Y,blur2_normalImg.pixel(x,y));
	  }
	}
      }
      else{
	for(int y=0; y<TOOLICON_L_HEIGHT; y++){
	  for(int x=0; x<TOOLICON_L_WIDTH; x++){
	    toolbarImg.setPixel(x+TOOLICON_L_X,y+TOOLICON_L_Y,blur_normalImg.pixel(x,y));
	  }
	}
      }
    }
    else if(canvas->tool == SHARPEN){
      for(int y=0; y<TOOLICON_L_HEIGHT; y++){
	for(int x=0; x<TOOLICON_L_WIDTH; x++){
	  toolbarImg.setPixel(x+TOOLICON_L_X,y+TOOLICON_L_Y,sharpen_normalImg.pixel(x,y));
	}
      }
    }

  }
  else{
    if(canvas->tool == PEN){
      if(canvas->fillFlag){
	for(int y=0; y<TOOLICON_L_HEIGHT; y++){
	  for(int x=0; x<TOOLICON_L_WIDTH; x++){
	    toolbarImg.setPixel(x+TOOLICON_L_X,y+TOOLICON_L_Y,fill_reverseImg.pixel(x,y));
	  }
	}
      }
      else if(canvas->bokeashiFlag && canvas->kasanenuriFlag){
	for(int y=0; y<TOOLICON_L_HEIGHT; y++){
	  for(int x=0; x<TOOLICON_L_WIDTH; x++){
	    toolbarImg.setPixel(x+TOOLICON_L_X,y+TOOLICON_L_Y,brush2_reverseImg.pixel(x,y));
	  }
	}
      }
      else if(canvas->bokeashiFlag && !canvas->kasanenuriFlag){
	for(int y=0; y<TOOLICON_L_HEIGHT; y++){
	  for(int x=0; x<TOOLICON_L_WIDTH; x++){
	    toolbarImg.setPixel(x+TOOLICON_L_X,y+TOOLICON_L_Y,brush_reverseImg.pixel(x,y));
	  }
	}
      }
      else{
	for(int y=0; y<TOOLICON_L_HEIGHT; y++){
	  for(int x=0; x<TOOLICON_L_WIDTH; x++){
	    toolbarImg.setPixel(x+TOOLICON_L_X,y+TOOLICON_L_Y,pencil_reverseImg.pixel(x,y));
	  }
	}
      }

    }
    else if(canvas->tool == ERASER){
      if(canvas->bokeashiFlag){
	for(int y=0; y<TOOLICON_L_HEIGHT; y++){
	  for(int x=0; x<TOOLICON_L_WIDTH; x++){
	    toolbarImg.setPixel(x+TOOLICON_L_X,y+TOOLICON_L_Y,eraser2_reverseImg.pixel(x,y));
	  }
	}
      }
      else{
	for(int y=0; y<TOOLICON_L_HEIGHT; y++){
	  for(int x=0; x<TOOLICON_L_WIDTH; x++){
	    toolbarImg.setPixel(x+TOOLICON_L_X,y+TOOLICON_L_Y,eraser_reverseImg.pixel(x,y));
	  }
	}
      }
    }
    else if(canvas->tool == BLUR){
      if(canvas->powerBlurFlag){    
      for(int y=0; y<TOOLICON_L_HEIGHT; y++){
	for(int x=0; x<TOOLICON_L_WIDTH; x++){
	  toolbarImg.setPixel(x+TOOLICON_L_X,y+TOOLICON_L_Y,blur2_reverseImg.pixel(x,y));
	}
      }
      }
      else{
      for(int y=0; y<TOOLICON_L_HEIGHT; y++){
	for(int x=0; x<TOOLICON_L_WIDTH; x++){
	  toolbarImg.setPixel(x+TOOLICON_L_X,y+TOOLICON_L_Y,blur_reverseImg.pixel(x,y));
	}
      }
      }
    }
    else if(canvas->tool == SHARPEN){
      for(int y=0; y<TOOLICON_L_HEIGHT; y++){
	for(int x=0; x<TOOLICON_L_WIDTH; x++){
	  toolbarImg.setPixel(x+TOOLICON_L_X,y+TOOLICON_L_Y,sharpen_reverseImg.pixel(x,y));
	}
      }
    }
  }

  if(canvas->zoomStyle == LUPE){
    for(int y=0; y<HANDICON_HEIGHT; y++){
      for(int x=0; x<HANDICON_WIDTH; x++){
	canvas->toolbar->toolbarImg.setPixel(HANDICON_X+x,HANDICON_Y+y,canvas->toolbar->handImg.pixel(x,y));      }
    }
  }
  else{
    if(canvas->handFlag){
      for(int y=HANDICON_Y; y<HANDICON_Y+HANDICON_HEIGHT; y++){
	for(int x=HANDICON_X; x<HANDICON_X+HANDICON_WIDTH; x++){
	  canvas->toolbar->toolbarImg.setPixel(x,y,canvas->toolbar->toolbar_reverseImg.pixel(x,y));
	}
      }
    }
    else{
      for(int y=HANDICON_Y; y<HANDICON_Y+HANDICON_HEIGHT; y++){
	for(int x=HANDICON_X; x<HANDICON_X+HANDICON_WIDTH; x++){
	  canvas->toolbar->toolbarImg.setPixel(x,y,canvas->toolbar->toolbar_normalImg.pixel(x,y));
	}
      }
    }
  }


  if(canvas->spoitSelectFlag && !canvas->spoitJogSelectFlag){
    for(int y=SPOIT_Y; y<SPOIT_Y+SPOIT_HEIGHT; y++){
      for(int x=SPOIT_X; x<SPOIT_X+SPOIT_WIDTH; x++){
	toolbarImg.setPixel(x,y,toolbar_reverseImg.pixel(x,y));
      }
    }
  }
  else{
    for(int y=SPOIT_Y; y<SPOIT_Y+SPOIT_HEIGHT; y++){
      for(int x=SPOIT_X; x<SPOIT_X+SPOIT_WIDTH; x++){
	toolbarImg.setPixel(x,y,toolbar_normalImg.pixel(x,y));
      }
    }
  }

  if(canvas->undoFlag){
    for(int y=UNDO_Y; y<UNDO_Y+UNDO_HEIGHT; y++){
      for(int x=UNDO_X; x<UNDO_X+UNDO_WIDTH; x++){
	toolbarImg.setPixel(x,y,toolbar_reverseImg.pixel(x,y));
      }
    }
  }
  else{
    for(int y=UNDO_Y; y<UNDO_Y+UNDO_HEIGHT; y++){
      for(int x=UNDO_X; x<UNDO_X+UNDO_WIDTH; x++){
	toolbarImg.setPixel(x,y,toolbar_normalImg.pixel(x,y));
      }
    }
  }

  if(opacityMinusSelectFlag){
    for(int y=OPACITY_MINUS_Y; y<OPACITY_MINUS_Y+OPACITY_MINUS_HEIGHT; y++){
      for(int x=OPACITY_MINUS_X; x<OPACITY_MINUS_X+OPACITY_MINUS_WIDTH; x++){
	toolbarImg.setPixel(x,y,toolbar_reverseImg.pixel(x,y));
      }
    }
  }
  else{
    for(int y=OPACITY_MINUS_Y; y<OPACITY_MINUS_Y+OPACITY_MINUS_HEIGHT; y++){
      for(int x=OPACITY_MINUS_X; x<OPACITY_MINUS_X+OPACITY_MINUS_WIDTH; x++){
	toolbarImg.setPixel(x,y,toolbar_normalImg.pixel(x,y));
      }
    }
  }

  if(opacityPlusSelectFlag){
    for(int y=OPACITY_PLUS_Y; y<OPACITY_PLUS_Y+OPACITY_PLUS_HEIGHT; y++){
      for(int x=OPACITY_PLUS_X; x<OPACITY_PLUS_X+OPACITY_PLUS_WIDTH; x++){
	toolbarImg.setPixel(x,y,toolbar_reverseImg.pixel(x,y));
      }
    }
  }
  else{
    for(int y=OPACITY_PLUS_Y; y<OPACITY_PLUS_Y+OPACITY_PLUS_HEIGHT; y++){
      for(int x=OPACITY_PLUS_X; x<OPACITY_PLUS_X+OPACITY_PLUS_WIDTH; x++){
	toolbarImg.setPixel(x,y,toolbar_normalImg.pixel(x,y));
      }
    }
  }


  if(canvas->fileMenuPanel->isVisible()){
    for(int y=FILEMENUICON_Y; y<FILEMENUICON_Y+FILEMENUICON_HEIGHT; y++){
      for(int x=FILEMENUICON_X; x<FILEMENUICON_X+FILEMENUICON_WIDTH; x++){
	canvas->toolbar->toolbarImg.setPixel(x,y,canvas->toolbar->toolbar_reverseImg.pixel(x,y));
      }
    }
  }
  else{
    for(int y=FILEMENUICON_Y; y<FILEMENUICON_Y+FILEMENUICON_HEIGHT; y++){
      for(int x=FILEMENUICON_X; x<FILEMENUICON_X+FILEMENUICON_WIDTH; x++){
	canvas->toolbar->toolbarImg.setPixel(x,y,canvas->toolbar->toolbar_normalImg.pixel(x,y));
      }
    }
  }

  if(canvas->zoomStyle == LUPE){
    for(int y=0; y<SELECTICON_HEIGHT; y++){
      for(int x=0; x<SELECTICON_WIDTH; x++){
	canvas->toolbar->toolbarImg.setPixel(SELECTICON_X+x,SELECTICON_Y+y,canvas->toolbar->selectHideImg.pixel(x,y));
      }
    }
  }
  else{
  if(canvas->selectPanel->isVisible()){
    for(int y=SELECTICON_Y; y<SELECTICON_Y+SELECTICON_HEIGHT; y++){
      for(int x=SELECTICON_X; x<SELECTICON_X+SELECTICON_WIDTH; x++){
	canvas->toolbar->toolbarImg.setPixel(x,y,canvas->toolbar->toolbar_reverseImg.pixel(x,y));
      }
    }
  }
  else{
    for(int y=SELECTICON_Y; y<SELECTICON_Y+SELECTICON_HEIGHT; y++){
      for(int x=SELECTICON_X; x<SELECTICON_X+SELECTICON_WIDTH; x++){
	canvas->toolbar->toolbarImg.setPixel(x,y,canvas->toolbar->toolbar_normalImg.pixel(x,y));
      }
    }
  }
  }

  if(canvas->scalePanel->isVisible()){
    for(int y=SCALEICON_Y; y<SCALEICON_Y+SCALEICON_HEIGHT; y++){
      for(int x=SCALEICON_X; x<SCALEICON_X+SCALEICON_WIDTH; x++){
	canvas->toolbar->toolbarImg.setPixel(x,y,canvas->toolbar->toolbar_reverseImg.pixel(x,y));
      }
    }
  }
  else{
    for(int y=SCALEICON_Y; y<SCALEICON_Y+SCALEICON_HEIGHT; y++){
      for(int x=SCALEICON_X; x<SCALEICON_X+SCALEICON_WIDTH; x++){
	canvas->toolbar->toolbarImg.setPixel(x,y,canvas->toolbar->toolbar_normalImg.pixel(x,y));
      }
    }
  }


  if(canvas->toolPanel->isVisible()){
    for(int y=TOOLICON_Y; y<TOOLICON_Y+TOOLICON_HEIGHT; y++){
      for(int x=TOOLICON_X; x<TOOLICON_X+TOOLICON_WIDTH; x++){
	canvas->toolbar->toolbarImg.setPixel(x,y,canvas->toolbar->toolbar_reverseImg.pixel(x,y));
      }
    }
  }
  else{
    for(int y=TOOLICON_Y; y<TOOLICON_Y+TOOLICON_HEIGHT; y++){
      for(int x=TOOLICON_X; x<TOOLICON_X+TOOLICON_WIDTH; x++){
	canvas->toolbar->toolbarImg.setPixel(x,y,canvas->toolbar->toolbar_normalImg.pixel(x,y));
      }
    }
  }


  if(canvas->drawModePanel->isVisible()){
    for(int y=DRAWMODEICON_Y; y<DRAWMODEICON_Y+DRAWMODEICON_HEIGHT; y++){
      for(int x=DRAWMODEICON_X; x<DRAWMODEICON_X+DRAWMODEICON_WIDTH; x++){
	toolbarImg.setPixel(x,y,toolbar_reverseImg.pixel(x,y));
      }
    }

    for(int y=0; y<DRAWMODEICON_HEIGHT-4; y++){
      for(int x=0; x<DRAWMODEICON_WIDTH-4; x++){
	toolbarImg.setPixel(x+DRAWMODEICON_X+2,y+2+DRAWMODEICON_Y,canvas->mode_reverseImg->pixel(x+15,y+3+canvas->drawMode*21));
      }
    }
  }
  else{
    for(int y=DRAWMODEICON_Y; y<DRAWMODEICON_Y+DRAWMODEICON_HEIGHT; y++){
      for(int x=DRAWMODEICON_X; x<DRAWMODEICON_X+DRAWMODEICON_WIDTH; x++){
	toolbarImg.setPixel(x,y,toolbar_normalImg.pixel(x,y));
      }
    }

    for(int y=0; y<DRAWMODEICON_HEIGHT-4; y++){
      for(int x=0; x<DRAWMODEICON_WIDTH-4; x++){
	toolbarImg.setPixel(x+DRAWMODEICON_X+2,y+2+DRAWMODEICON_Y,canvas->mode_normalImg->pixel(x+15,y+3+canvas->drawMode*21));
      }
    }
  }


  QPixmap toolbarPix = QPixmap();
  toolbarPix.convertFromImage(toolbarImg, 0);


  QPainter p2;
  p2.begin( &toolbarPix );
  p2.setFont(QFont("lcfont", 8));
  p2.setPen( black );
  QString tmpStr = "0";
  tmpStr = tmpStr.sprintf( "%d", canvas->currentlayer+1);
  p2.drawText(QRect(6,6, 9 ,10),Qt::AlignCenter , tmpStr);
  if(canvas->canvasType == VGA){
    tmpStr = tmpStr.sprintf( "%d", canvas->layernum);
  }
  else if(canvas->canvasType == QVGA){
    tmpStr = tmpStr.sprintf( "%dq", canvas->layernum);
  }
  p2.drawText(QRect(17,16, 11 ,10),Qt::AlignCenter , tmpStr);

  if(canvas->useColorMatrix){
    tmpStr = tmpStr.sprintf( "ca");
  }
  else{
    tmpStr = tmpStr.sprintf( "  ");
  }
  p2.drawText(QRect(MESHICON_X,MESHICON_Y + MESHICON_HEIGHT-1 , 9 ,10),Qt::AlignLeft , tmpStr);


  p2.end();




  repaint_left = 0;
  repaint_top = 0;
  repaint_width = 0;
  repaint_height = 0;

  setBackgroundPixmap(toolbarPix);


  /*
    repaint_left = 0;
    repaint_top = 0;
    repaint_width = 480;
    repaint_height = 62;

    repaint();
  */
}


void MyToolBar::changeToEraser(){

  if(canvas->bokeashiFlag){
    for(int y=0; y<TOOLICON_L_HEIGHT; y++){
      for(int x=0; x<TOOLICON_L_WIDTH; x++){
	toolbarImg.setPixel(x+TOOLICON_L_X,y+TOOLICON_L_Y,eraser2_reverseImg.pixel(x,y));
      }
    }
  }
  else{
    for(int y=0; y<TOOLICON_L_HEIGHT; y++){
      for(int x=0; x<TOOLICON_L_WIDTH; x++){
      toolbarImg.setPixel(x+TOOLICON_L_X,y+TOOLICON_L_Y,eraser_reverseImg.pixel(x,y));
      }
    }
  }

  canvas->tool = ERASER;

  canvas->pensizePanel->myReset();

  for(int y=COLORAREA3_Y; y<COLORAREA3_Y+COLORAREA3_HEIGHT; y++){
    for(int x=COLORAREA3_X; x<COLORAREA3_X+COLORAREA3_WIDTH; x++){
      toolbarImg.setPixel(x,y, 0xffffffff);      
    }
  }

  for(int x=OPACITY_X; x< OPACITY_X + OPACITY_WIDTH + 4; x++){
    toolbarImg.setPixel(x,OPACITY_Y+1, 0xff9B9B9B);
    toolbarImg.setPixel(x,OPACITY_Y+2, 0xff9B9B9B);
    toolbarImg.setPixel(x,OPACITY_Y+3, 0xff9B9B9B);
    toolbarImg.setPixel(x,OPACITY_Y+4, 0xff9B9B9B);
  }

  changeOpacityArea(OPACITY_X+canvas->eraseralpha);
  changeColorArea();

  for(int x=OPACITY_X+75; x<OPACITY_X+OPACITY_WIDTH-7;x++)
    for(int y=OPACITY_Y-11; y<OPACITY_Y;y++)
      toolbarImg.setPixel(x,y,0xff9B9B9B);

  //ツールバー画像の表示
  QPixmap toolbarPix = QPixmap();
  toolbarPix.convertFromImage(toolbarImg, 0);

  QString tmpStr = "   ";
  tmpStr = tmpStr.sprintf("%d",canvas->eraseralpha);
  QPainter p;
  p.begin( &toolbarPix );
  p.setPen( black );
  p.setFont(QFont("lcfont", 8));
  p.drawText(QRect(OPACITY_X,OPACITY_Y-11, OPACITY_WIDTH-7, 12),Qt::AlignRight , tmpStr);
  p.end();  
  toolbarImg = toolbarPix.convertToImage();
  repaint_left = 0;
  repaint_top = 0;
  repaint_width = 0;
  repaint_height = 0;

  setBackgroundPixmap(toolbarPix);
  /*
    repaint_left = 0;
    repaint_top = 0;
    repaint_width = 640;
    repaint_height = 480;

    repaint();
  */
}


void MyToolBar::changeToDrawtool(){

  if(canvas->fillFlag){
    for(int y=0; y<TOOLICON_L_HEIGHT; y++){
      for(int x=0; x<TOOLICON_L_WIDTH; x++){
	toolbarImg.setPixel(x+TOOLICON_L_X,y+TOOLICON_L_Y,fill_reverseImg.pixel(x,y));
      }
    }
  }
  else if(canvas->bokeashiFlag && canvas->kasanenuriFlag){
    for(int y=0; y<TOOLICON_L_HEIGHT; y++){
      for(int x=0; x<TOOLICON_L_WIDTH; x++){
	toolbarImg.setPixel(x+TOOLICON_L_X,y+TOOLICON_L_Y,brush2_reverseImg.pixel(x,y));
      }
    }
  }
  else if(canvas->bokeashiFlag && !canvas->kasanenuriFlag){
    for(int y=0; y<TOOLICON_L_HEIGHT; y++){
      for(int x=0; x<TOOLICON_L_WIDTH; x++){
	toolbarImg.setPixel(x+TOOLICON_L_X,y+TOOLICON_L_Y,brush_reverseImg.pixel(x,y));
      }
    }
  }
  else{
    for(int y=0; y<TOOLICON_L_HEIGHT; y++){
      for(int x=0; x<TOOLICON_L_WIDTH; x++){
	toolbarImg.setPixel(x+TOOLICON_L_X,y+TOOLICON_L_Y,pencil_reverseImg.pixel(x,y));
      }
    }
  }

  canvas->tool = PEN;

  canvas->pensizePanel->myReset();

  changeColorArea();

  for(int x=OPACITY_X; x< OPACITY_X + OPACITY_WIDTH + 4; x++){
    toolbarImg.setPixel(x,OPACITY_Y+1, 0xff9B9B9B);
    toolbarImg.setPixel(x,OPACITY_Y+2, 0xff9B9B9B);
    toolbarImg.setPixel(x,OPACITY_Y+3, 0xff9B9B9B);
    toolbarImg.setPixel(x,OPACITY_Y+4, 0xff9B9B9B);
  }

  changeOpacityArea(OPACITY_X+canvas->penalpha);
  changeColorArea();

  for(int x=OPACITY_X+75; x<OPACITY_X+OPACITY_WIDTH-7;x++)
    for(int y=OPACITY_Y-11; y<OPACITY_Y;y++)
      toolbarImg.setPixel(x,y,0xff9B9B9B);

  //ツールバー画像の表示
  QPixmap toolbarPix = QPixmap();
  toolbarPix.convertFromImage(toolbarImg, 0);

  QString tmpStr = "   ";

  tmpStr = tmpStr.sprintf("%d",canvas->penalpha);


  QPainter p;
  p.begin( &toolbarPix );
  p.setPen( black );
  p.setFont(QFont("lcfont", 8));
  p.drawText(QRect(OPACITY_X,OPACITY_Y-11, OPACITY_WIDTH-7, 12),Qt::AlignRight , tmpStr);
  p.end();  
  toolbarImg = toolbarPix.convertToImage();
  repaint_left = 0;
  repaint_top = 0;
  repaint_width = 0;
  repaint_height = 0;

  setBackgroundPixmap(toolbarPix);
  /*
    repaint_left = 0;
    repaint_top = 0;
    repaint_width = 480;
    repaint_height = 62;

    repaint();
  */
}

void MyToolBar::changeToBlur(){

  if(canvas->powerBlurFlag){
    for(int y=0; y<TOOLICON_L_HEIGHT; y++){
      for(int x=0; x<TOOLICON_L_WIDTH; x++){
	toolbarImg.setPixel(x+TOOLICON_L_X,y+TOOLICON_L_Y,blur2_reverseImg.pixel(x,y));
      }
    }
  }
  else{
    for(int y=0; y<TOOLICON_L_HEIGHT; y++){
      for(int x=0; x<TOOLICON_L_WIDTH; x++){
	toolbarImg.setPixel(x+TOOLICON_L_X,y+TOOLICON_L_Y,blur_reverseImg.pixel(x,y));
      }
    }
  }

  canvas->tool = BLUR;

  canvas->pensizePanel->myReset();

  changeColorArea();

  for(int x=OPACITY_X; x< OPACITY_X + OPACITY_WIDTH + 4; x++){
    toolbarImg.setPixel(x,OPACITY_Y+1, 0xff9B9B9B);
    toolbarImg.setPixel(x,OPACITY_Y+2, 0xff9B9B9B);
    toolbarImg.setPixel(x,OPACITY_Y+3, 0xff9B9B9B);
    toolbarImg.setPixel(x,OPACITY_Y+4, 0xff9B9B9B);
  }

  changeOpacityArea(OPACITY_X+canvas->bluralpha);
  changeColorArea();

  for(int x=OPACITY_X+75; x<OPACITY_X+OPACITY_WIDTH-7;x++)
    for(int y=OPACITY_Y-11; y<OPACITY_Y;y++)
      toolbarImg.setPixel(x,y,0xff9B9B9B);

  //ツールバー画像の表示
  QPixmap toolbarPix = QPixmap();
  toolbarPix.convertFromImage(toolbarImg, 0);

  QString tmpStr = "   ";
  tmpStr = tmpStr.sprintf("%d",canvas->bluralpha);

  QPainter p;
  p.begin( &toolbarPix );
  p.setPen( black );
  p.setFont(QFont("lcfont", 8));
  p.drawText(QRect(OPACITY_X,OPACITY_Y-11, OPACITY_WIDTH-7, 12),Qt::AlignRight , tmpStr);
  p.end();  
  toolbarImg = toolbarPix.convertToImage();
  repaint_left = 0;
  repaint_top = 0;
  repaint_width = 0;
  repaint_height = 0;

  setBackgroundPixmap(toolbarPix);

}



void MyToolBar::changeToSharpen(){

  for(int y=0; y<TOOLICON_L_HEIGHT; y++){
    for(int x=0; x<TOOLICON_L_WIDTH; x++){
      toolbarImg.setPixel(x+TOOLICON_L_X,y+TOOLICON_L_Y,sharpen_reverseImg.pixel(x,y));
    }
  }

  canvas->tool = SHARPEN;


  canvas->pensizePanel->myReset();



  for(int x=OPACITY_X; x< OPACITY_X + OPACITY_WIDTH + 4; x++){
    toolbarImg.setPixel(x,OPACITY_Y+1, 0xff9B9B9B);
    toolbarImg.setPixel(x,OPACITY_Y+2, 0xff9B9B9B);
    toolbarImg.setPixel(x,OPACITY_Y+3, 0xff9B9B9B);
    toolbarImg.setPixel(x,OPACITY_Y+4, 0xff9B9B9B);
  }

  changeOpacityArea(OPACITY_X+canvas->sharpenalpha);
  changeColorArea();

  for(int x=OPACITY_X+75; x<OPACITY_X+OPACITY_WIDTH-7;x++)
    for(int y=OPACITY_Y-11; y<OPACITY_Y;y++)
      toolbarImg.setPixel(x,y,0xff9B9B9B);

  //ツールバー画像の表示
  QPixmap toolbarPix = QPixmap();
  toolbarPix.convertFromImage(toolbarImg, 0);

  QString tmpStr = "   ";
  tmpStr = tmpStr.sprintf("%d",canvas->sharpenalpha);

  QPainter p;
  p.begin( &toolbarPix );
  p.setPen( black );
  p.setFont(QFont("lcfont", 8));
  p.drawText(QRect(OPACITY_X,OPACITY_Y-11, OPACITY_WIDTH-7, 12),Qt::AlignRight , tmpStr);
  p.end();  
  toolbarImg = toolbarPix.convertToImage();
  repaint_left = 0;
  repaint_top = 0;
  repaint_width = 0;
  repaint_height = 0;

  setBackgroundPixmap(toolbarPix);

}


void MyToolBar::changeOpacityArea(int x){

  int tmpAlpha;
  int tmpAlpha2;

  if(x<OPACITY_X)
    tmpAlpha = 0;
  else if(x>OPACITY_X+OPACITY_WIDTH)
    tmpAlpha = 100;	
  else
    tmpAlpha = x - (OPACITY_X);

  tmpAlpha2 = (int)((double)tmpAlpha*2.56);
  if(tmpAlpha2>256){
    tmpAlpha2 = 256;
  }


  toolbarImg.setPixel(OPACITY_X+1+tmpAlpha,OPACITY_Y+1, 0xff000000);
  toolbarImg.setPixel(OPACITY_X+1+tmpAlpha,OPACITY_Y+2, 0xffffffff);
  toolbarImg.setPixel(OPACITY_X+1+tmpAlpha,OPACITY_Y+3, 0xffffffff);
      toolbarImg.setPixel(OPACITY_X+1+tmpAlpha,OPACITY_Y+4, 0xff000000);

      toolbarImg.setPixel(OPACITY_X  +tmpAlpha,OPACITY_Y+2, 0xff000000);
      toolbarImg.setPixel(OPACITY_X  +tmpAlpha,OPACITY_Y+3, 0xff000000);
      toolbarImg.setPixel(OPACITY_X  +tmpAlpha,OPACITY_Y+4, 0xff000000);

      toolbarImg.setPixel(OPACITY_X+2+tmpAlpha,OPACITY_Y+2, 0xff000000);
      toolbarImg.setPixel(OPACITY_X+2+tmpAlpha,OPACITY_Y+3, 0xff000000);
      toolbarImg.setPixel(OPACITY_X+2+tmpAlpha,OPACITY_Y+4, 0xff000000);

      if(canvas->tool == ERASER){
	canvas->eraseralpha=tmpAlpha;
	canvas->eraseralpha2=tmpAlpha2;
      }
      else if(canvas->tool == PEN){
	canvas->penalpha=tmpAlpha;
	canvas->penalpha2=tmpAlpha2;
      }
      else if(canvas->tool == BLUR){
	canvas->bluralpha=tmpAlpha;
	canvas->bluralpha2=tmpAlpha2;
      }
      else if(canvas->tool == SHARPEN){
	canvas->sharpenalpha=tmpAlpha;
	canvas->sharpenalpha2=tmpAlpha2;
      }

}


void MyToolBar::changeOpacityArea2(int tmpOpacity){
  QPainter p;
  QString tmpStr = "   ";
  int tmpAlpha = 0;

    if(canvas->tool == ERASER){
      tmpAlpha = canvas->eraseralpha;
    }
    else if(canvas->tool == PEN){
      tmpAlpha = canvas->penalpha;
    }
    else if(canvas->tool == BLUR){
      tmpAlpha = canvas->bluralpha;
    }
    else if(canvas->tool == SHARPEN){
      tmpAlpha = canvas->sharpenalpha;
    }

      toolbarImg.setPixel(OPACITY_X+1+tmpAlpha,OPACITY_Y+1, 0xff9B9B9B);
      toolbarImg.setPixel(OPACITY_X+1+tmpAlpha,OPACITY_Y+2, 0xff9B9B9B);
      toolbarImg.setPixel(OPACITY_X+1+tmpAlpha,OPACITY_Y+3, 0xff9B9B9B);
      toolbarImg.setPixel(OPACITY_X+1+tmpAlpha,OPACITY_Y+4, 0xff9B9B9B);

      toolbarImg.setPixel(OPACITY_X  +tmpAlpha,OPACITY_Y+2, 0xff9B9B9B);
      toolbarImg.setPixel(OPACITY_X  +tmpAlpha,OPACITY_Y+3, 0xff9B9B9B);
      toolbarImg.setPixel(OPACITY_X  +tmpAlpha,OPACITY_Y+4, 0xff9B9B9B);

      toolbarImg.setPixel(OPACITY_X+2+tmpAlpha,OPACITY_Y+2, 0xff9B9B9B);
      toolbarImg.setPixel(OPACITY_X+2+tmpAlpha,OPACITY_Y+3, 0xff9B9B9B);
      toolbarImg.setPixel(OPACITY_X+2+tmpAlpha,OPACITY_Y+4, 0xff9B9B9B);

      changeOpacityArea(tmpOpacity);
      changeColorArea();

      for(int x=OPACITY_X+75; x<OPACITY_X+OPACITY_WIDTH-7;x++)
	for(int y=OPACITY_Y-11; y<OPACITY_Y;y++)
	  toolbarImg.setPixel(x,y,0xff9B9B9B);

      //ツールバー画像の表示
      QPixmap toolbarPix = QPixmap();
      toolbarPix.convertFromImage(toolbarImg, 0);
      
      if(canvas->tool == ERASER){
	tmpStr = tmpStr.sprintf("%d",canvas->eraseralpha);
      }
      else if(canvas->tool == PEN){
	tmpStr = tmpStr.sprintf("%d",canvas->penalpha);
      }
      else if(canvas->tool == BLUR){
	tmpStr = tmpStr.sprintf("%d",canvas->bluralpha);
      }
      else if(canvas->tool == SHARPEN){
	tmpStr = tmpStr.sprintf("%d",canvas->sharpenalpha);
      }
      p.begin( &toolbarPix );
      p.setPen( black );
      p.setFont(QFont("lcfont", 8));
      p.drawText(QRect(OPACITY_X,OPACITY_Y-11, OPACITY_WIDTH-7, 12),Qt::AlignRight , tmpStr);
      p.end();  

            toolbarImg = toolbarPix.convertToImage();
      //      setBackgroundPixmap(toolbarPix);

  repaint_left = OPACITY_X-2;
  repaint_top = OPACITY_Y-12;
  repaint_width = OPACITY_WIDTH+5;
  repaint_height = OPACITY_HEIGHT+12;
  QRegion r(repaint_left,repaint_top,repaint_width,repaint_height,QRegion::Rectangle);
  repaint(r, false);

}

void MyToolBar::changeColorArea(){

  unsigned int tmpR=0, tmpG=0, tmpB=0;
  unsigned int tmpbgR=0, tmpbgG=0, tmpbgB=0;

  for(int y=COLORAREA2_Y; y<COLORAREA2_Y+COLORAREA2_HEIGHT; y++){
    for(int x=COLORAREA2_X; x<COLORAREA2_X+COLORAREA2_WIDTH; x++){
      if(canvas->useColorMatrix)
	toolbarImg.setPixel(x,y, 
			    0xff000000
			    | (( canvas->colorMatrix[0][(canvas->bgcolor>>16) & 0x000000ff] << 16) & 0x00ff0000 )
			    | (( canvas->colorMatrix[1][(canvas->bgcolor>>8) & 0x000000ff] << 8) & 0x0000ff00 )
			    | (( canvas->colorMatrix[2][(canvas->bgcolor) & 0x000000ff] ) & 0x000000ff )
			    );
      else
	toolbarImg.setPixel(x,y, canvas->bgcolor);
    }
  }

  for(int y=COLORAREA_Y; y<COLORAREA_Y+COLORAREA_HEIGHT; y++){
    for(int x=COLORAREA_X; x<COLORAREA_X+COLORAREA_WIDTH; x++){
      if(canvas->useColorMatrix)
	toolbarImg.setPixel(x,y, 
			    0xff000000
			    | (( canvas->colorMatrix[0][(canvas->pencolor>>16) & 0x000000ff] << 16) & 0x00ff0000 )
			    | (( canvas->colorMatrix[1][(canvas->pencolor>>8) & 0x000000ff] << 8) & 0x0000ff00 )
			    | (( canvas->colorMatrix[2][(canvas->pencolor) & 0x000000ff] ) & 0x000000ff )
			    );
      else
	toolbarImg.setPixel(x,y, canvas->pencolor);      
    }
  }

  if(canvas->useColorMatrix){
    tmpR = canvas->colorMatrix[0][0x000000ff -  ((0x000000ff - ((canvas->pencolor>>16)&0x000000ff))*canvas->penalpha2>>8)];
    tmpG = canvas->colorMatrix[1][0x000000ff -  ((0x000000ff - ((canvas->pencolor>>8)&0x000000ff))*canvas->penalpha2>>8)];
    tmpB = canvas->colorMatrix[2][0x000000ff -  ((0x000000ff - ((canvas->pencolor)&0x000000ff))*canvas->penalpha2>>8)];
    tmpbgR = canvas->colorMatrix[0][0x000000ff -  ((0x000000ff - ((canvas->bgcolor>>16)&0x000000ff))*canvas->penalpha2>>8)];
    tmpbgG = canvas->colorMatrix[1][0x000000ff -  ((0x000000ff - ((canvas->bgcolor>>8)&0x000000ff))*canvas->penalpha2>>8)];
    tmpbgB = canvas->colorMatrix[2][0x000000ff -  ((0x000000ff - ((canvas->bgcolor)&0x000000ff))*canvas->penalpha2>>8)];
  }
  else{
    tmpR = 0x000000ff -  ((0x000000ff - ((canvas->pencolor>>16)&0x000000ff))*canvas->penalpha2>>8);
    tmpG = 0x000000ff -  ((0x000000ff - ((canvas->pencolor>>8)&0x000000ff))*canvas->penalpha2>>8);
    tmpB = 0x000000ff -  ((0x000000ff - ((canvas->pencolor)&0x000000ff))*canvas->penalpha2>>8);
    tmpbgR = 0x000000ff -  ((0x000000ff - ((canvas->bgcolor>>16)&0x000000ff))*canvas->penalpha2>>8);
    tmpbgG = 0x000000ff -  ((0x000000ff - ((canvas->bgcolor>>8)&0x000000ff))*canvas->penalpha2>>8);
    tmpbgB = 0x000000ff -  ((0x000000ff - ((canvas->bgcolor)&0x000000ff))*canvas->penalpha2>>8);
  }


  if(canvas->tool != ERASER){
    for(int y=COLORAREA3_Y; y<COLORAREA3_Y+COLORAREA3_HEIGHT; y++){
      for(int x=COLORAREA3_X; x<COLORAREA3_X+COLORAREA3_WIDTH; x++){

	if(canvas->meshpattern == MY_BLACK){
	  toolbarImg.setPixel(x,y,0xff000000 | ((tmpR<<16)&0x00ff0000) | ((tmpG<<8)&0x0000ff00) | ((tmpB)&0x000000ff));
	}
	else if(canvas->meshpattern == MY_DARK_GRAY){
	  if( (y%2 == 0) 
	      || ((x%2 == 1) && (y%2 == 1))){	
	    toolbarImg.setPixel(x,y,0xff000000 | ((tmpR<<16)&0x00ff0000) | ((tmpG<<8)&0x0000ff00) | ((tmpB)&0x000000ff));
	  }
	  else if(canvas->overwrite == OVERWRITE){
	    toolbarImg.setPixel(x,y,0xff000000 | ((tmpbgR<<16)&0x00ff0000) | ((tmpbgG<<8)&0x0000ff00) | ((tmpbgB)&0x000000ff));
	  }
	  else{
	    toolbarImg.setPixel(x,y,0xffffffff);
	  }
	}
	else if(canvas->meshpattern == MY_GRAY){
	  /* X,Y座標がともに奇数、あるいはともに偶数の時に描画 */
	  if( ((x%2 == 0) && (y%2 == 0)) 
	      || ((x%2 == 1) && (y%2 == 1))){
	    toolbarImg.setPixel(x,y,0xff000000 | ((tmpR<<16)&0x00ff0000) | ((tmpG<<8)&0x0000ff00) | ((tmpB)&0x000000ff));
	  }
	  else if(canvas->overwrite == OVERWRITE){
	    toolbarImg.setPixel(x,y,0xff000000 | ((tmpbgR<<16)&0x00ff0000) | ((tmpbgG<<8)&0x0000ff00) | ((tmpbgB)&0x000000ff));
	  }
	  else{
	    toolbarImg.setPixel(x,y,0xffffffff);
	  }
	}
	else if(canvas->meshpattern == MY_FENCE_GRAY){
	  if( ((x%2 == 0) && (y%2 == 0))
	      || ((x%4 == 1) && (y%4 == 1))
	      || ((x%4 == 3) && (y%4 == 3)) ) {
	    toolbarImg.setPixel(x,y,0xff000000 | ((tmpR<<16)&0x00ff0000) | ((tmpG<<8)&0x0000ff00) | ((tmpB)&0x000000ff));
	  }
	  else if(canvas->overwrite == OVERWRITE){
	    toolbarImg.setPixel(x,y,0xff000000 | ((tmpbgR<<16)&0x00ff0000) | ((tmpbgG<<8)&0x0000ff00) | ((tmpbgB)&0x000000ff));
	  }
	  else{
	    toolbarImg.setPixel(x,y,0xffffffff);
	  }
	}
	else if(canvas->meshpattern == MY_GRID_GRAY){
	  /* X座標が偶数、かつY座標が偶数の時に描画 */
	  if( (x%2 == 0) && (y%2 == 0)) {
	    toolbarImg.setPixel(x,y,0xff000000 | ((tmpR<<16)&0x00ff0000) | ((tmpG<<8)&0x0000ff00) | ((tmpB)&0x000000ff));
	  }
	  else if(canvas->overwrite == OVERWRITE){
	    toolbarImg.setPixel(x,y,0xff000000 | ((tmpbgR<<16)&0x00ff0000) | ((tmpbgG<<8)&0x0000ff00) | ((tmpbgB)&0x000000ff));
	  }
	  else{
	    toolbarImg.setPixel(x,y,0xffffffff);
	  }
	}
	else if(canvas->meshpattern == MY_LIGHT_GRAY){
	  /* Y座標が4の倍数かつX座標が4の倍数の時、
	     　　　あるいは、Y座標が4の倍数+2かつX座標が4の倍数+2の時に描画 */
	  if( ((x%4 == 0) && (y%4 == 0))
	      || ((x%4 == 2) && (y%4 == 2))   ) {
	    toolbarImg.setPixel(x,y,0xff000000 | ((tmpR<<16)&0x00ff0000) | ((tmpG<<8)&0x0000ff00) | ((tmpB)&0x000000ff));
	  }
	  else if(canvas->overwrite == OVERWRITE){
	    toolbarImg.setPixel(x,y,0xff000000 | ((tmpbgR<<16)&0x00ff0000) | ((tmpbgG<<8)&0x0000ff00) | ((tmpbgB)&0x000000ff));
	  }
	  else{
	    toolbarImg.setPixel(x,y,0xffffffff);
	  }
	}	
	else if(canvas->meshpattern == MY_LEFT_INCLINE){
	  if( x%4 == y%4 ){
	    toolbarImg.setPixel(x,y,0xff000000 | ((tmpR<<16)&0x00ff0000) | ((tmpG<<8)&0x0000ff00) | ((tmpB)&0x000000ff));
	  }
	  else if(canvas->overwrite == OVERWRITE){
	    toolbarImg.setPixel(x,y,0xff000000 | ((tmpbgR<<16)&0x00ff0000) | ((tmpbgG<<8)&0x0000ff00) | ((tmpbgB)&0x000000ff));
	  }
	  else{
	    toolbarImg.setPixel(x,y,0xffffffff);
	  }
	}	
	else if(canvas->meshpattern == MY_RIGHT_INCLINE){
	  if( ((x%4 == 0) && (y%4 == 0))
	      || ((x%4 == 3) && (y%4 == 1))
	      || ((x%4 == 2) && (y%4 == 2))
	      || ((x%4 == 1) && (y%4 == 3)) ) {
	    toolbarImg.setPixel(x,y,0xff000000 | ((tmpR<<16)&0x00ff0000) | ((tmpG<<8)&0x0000ff00) | ((tmpB)&0x000000ff));
	  }
	  else if(canvas->overwrite == OVERWRITE){
	    toolbarImg.setPixel(x,y,0xff000000 | ((tmpbgR<<16)&0x00ff0000) | ((tmpbgG<<8)&0x0000ff00) | ((tmpbgB)&0x000000ff));
	  }
	  else{
	    toolbarImg.setPixel(x,y,0xffffffff);
	  }
	}	
	else if(canvas->meshpattern == MY_BORDER){
	  if( y%2 == 0){
	    toolbarImg.setPixel(x,y,0xff000000 | ((tmpR<<16)&0x00ff0000) | ((tmpG<<8)&0x0000ff00) | ((tmpB)&0x000000ff));
	  }
	  else if(canvas->overwrite == OVERWRITE){
	    toolbarImg.setPixel(x,y,0xff000000 | ((tmpbgR<<16)&0x00ff0000) | ((tmpbgG<<8)&0x0000ff00) | ((tmpbgB)&0x000000ff));
	  }
	  else{
	    toolbarImg.setPixel(x,y,0xffffffff);
	  }
	}
	else if(canvas->meshpattern == MY_STRIPE){
	  if( x%2 == 0){
	    toolbarImg.setPixel(x,y,0xff000000 | ((tmpR<<16)&0x00ff0000) | ((tmpG<<8)&0x0000ff00) | ((tmpB)&0x000000ff));
	  }
	  else if(canvas->overwrite == OVERWRITE){
	    toolbarImg.setPixel(x,y,0xff000000 | ((tmpbgR<<16)&0x00ff0000) | ((tmpbgG<<8)&0x0000ff00) | ((tmpbgB)&0x000000ff));
	  }
	  else{
	    toolbarImg.setPixel(x,y,0xffffffff);
	  }
	}	

      }
    }
  }


}


void MyToolBar::reverseSBCursor(){
      // 現在のSB位置表示を反転して戻す
  if((canvas->fgSa*169/COLORDIV) >= 1)
    toolbarImg.setPixel(SBAREA_X+(int)(canvas->fgSa*169/COLORDIV)-1,SBAREA_Y+57-(int)(canvas->fgBr*57/COLORDIV), (~(toolbarImg.pixel(SBAREA_X+(int)(canvas->fgSa*169/COLORDIV)-1,SBAREA_Y+57-(int)(canvas->fgBr*57/COLORDIV))))|0xff000000 );
  if((canvas->fgSa*169/COLORDIV) >= 2)
    toolbarImg.setPixel(SBAREA_X+(int)(canvas->fgSa*169/COLORDIV)-2,SBAREA_Y+57-(int)(canvas->fgBr*57/COLORDIV), (~(toolbarImg.pixel(SBAREA_X+(int)(canvas->fgSa*169/COLORDIV)-2,SBAREA_Y+57-(int)(canvas->fgBr*57/COLORDIV))))|0xff000000 );
  if(57-(int)(canvas->fgBr*57/COLORDIV) >= 1)
    toolbarImg.setPixel(SBAREA_X+(int)(canvas->fgSa*169/COLORDIV),SBAREA_Y+57-(int)(canvas->fgBr*57/COLORDIV)-1, (~(toolbarImg.pixel(SBAREA_X+(int)(canvas->fgSa*169/COLORDIV),SBAREA_Y+57-(int)(canvas->fgBr*57/COLORDIV)-1)))|0xff000000);
  if(57-(int)(canvas->fgBr*57/COLORDIV) >= 2)
    toolbarImg.setPixel(SBAREA_X+(int)(canvas->fgSa*169/COLORDIV),SBAREA_Y+57-(int)(canvas->fgBr*57/COLORDIV)-2, (~(toolbarImg.pixel(SBAREA_X+(int)(canvas->fgSa*169/COLORDIV),SBAREA_Y+57-(int)(canvas->fgBr*57/COLORDIV)-2)))|0xff000000);
  if((canvas->fgSa*169/COLORDIV) <= 168)
    toolbarImg.setPixel(SBAREA_X+(int)(canvas->fgSa*169/COLORDIV)+1,SBAREA_Y+57-(int)(canvas->fgBr*57/COLORDIV), (~(toolbarImg.pixel(SBAREA_X+(int)(canvas->fgSa*169/COLORDIV)+1,SBAREA_Y+57-(int)(canvas->fgBr*57/COLORDIV))))|0xff000000);
  if((canvas->fgSa*169/COLORDIV) <= 167)
    toolbarImg.setPixel(SBAREA_X+(int)(canvas->fgSa*169/COLORDIV)+2,SBAREA_Y+57-(int)(canvas->fgBr*57/COLORDIV), (~(toolbarImg.pixel(SBAREA_X+(int)(canvas->fgSa*169/COLORDIV)+2,SBAREA_Y+57-(int)(canvas->fgBr*57/COLORDIV))))|0xff000000);
  if(57-(int)(canvas->fgBr*57/COLORDIV) <= 56)
    toolbarImg.setPixel(SBAREA_X+(int)(canvas->fgSa*169/COLORDIV),SBAREA_Y+57-(int)(canvas->fgBr*57/COLORDIV)+1, (~(toolbarImg.pixel(SBAREA_X+(int)(canvas->fgSa*169/COLORDIV),SBAREA_Y+57-(int)(canvas->fgBr*57/COLORDIV)+1)))|0xff000000);
  if(57-(int)(canvas->fgBr*57/COLORDIV) <= 55)
    toolbarImg.setPixel(SBAREA_X+(int)(canvas->fgSa*169/COLORDIV),SBAREA_Y+57-(int)(canvas->fgBr*57/COLORDIV)+2, (~(toolbarImg.pixel(SBAREA_X+(int)(canvas->fgSa*169/COLORDIV),SBAREA_Y+57-(int)(canvas->fgBr*57/COLORDIV)+2)))|0xff000000);

}



void MyToolBar::changeSBArea(){

  int tmpSa=0, tmpBr=0;

  tmpSa = canvas->fgSa;
  tmpBr = canvas->fgBr;

  
  for(int y=0; y<SBAREA_HEIGHT; y++){
    for(int x=0; x<SBAREA_WIDTH; x++){
      canvas->fgSa=x*COLORDIV/169;
      canvas->fgBr=(57-y)*COLORDIV/57;
      canvas->HSB2RGB();

      if(canvas->useColorMatrix){
	toolbarImg.setPixel(x+SBAREA_X,y+SBAREA_Y,
			    0xff000000 
			    | ((canvas->colorMatrix[0][((canvas->pencolor >> 16) & 0x000000ff)] << 16) & 0x00ff0000)
			    | ((canvas->colorMatrix[1][((canvas->pencolor >> 8) & 0x000000ff)] << 8) & 0x0000ff00)
			    | ((canvas->colorMatrix[2][((canvas->pencolor ) & 0x000000ff)] ) & 0x000000ff)
			    );
      }
      else{
	toolbarImg.setPixel(x+SBAREA_X,y+SBAREA_Y, canvas->pencolor);
      }
    }
  }

  canvas->fgSa = tmpSa;
  canvas->fgBr = tmpBr;
  canvas->HSB2RGBd();


  reverseSBCursor();

}


void MyToolBar::mousePressEvent( QMouseEvent *e )
{
  unsigned int tmpColor = canvas->bgcolor;
  QPainter p;
  QString tmpStr = "   ";
  int tmpAlpha = 0;


  opacityMinusSelectFlag = false;
  opacityPlusSelectFlag = false;

  sbSelectingFlag = false;
  opacitySelectingFlag = false;

  if(e->y()>=CHANGECOLORAREA_Y && e->y()< CHANGECOLORAREA_Y+CHANGECOLORAREA_HEIGHT){
    if(e->x()>=CHANGECOLORAREA_X && e->x()< CHANGECOLORAREA_X+CHANGECOLORAREA_WIDTH){
      
      canvas->bgcolor = canvas->pencolor;
      canvas->pencolor = tmpColor;

      changeColorArea();

      //ツールバー画像の表示
      QPixmap toolbarPix = QPixmap();
      toolbarPix.convertFromImage(toolbarImg, 0);
      repaint_left = 0;
      repaint_top = 0;
      repaint_width = 0;
      repaint_height = 0;

      setBackgroundPixmap(toolbarPix);

      return;
    }
  }


  if(e->y()>=TOOLICON_L_Y && e->y()<TOOLICON_L_Y+TOOLICON_L_HEIGHT){
    if(e->x()>=TOOLICON_L_X && e->x()<TOOLICON_L_X+TOOLICON_L_WIDTH){
      if(!canvas->handFlag && !canvas->spoitSelectFlag){
	if(canvas->tool == PEN && !canvas->fillFlag){
	  changeToEraser();
	}
	else if(canvas->tool == ERASER){
	  changeToDrawtool();
	}
      }
      canvas->handFlag = false;
      canvas->spoitSelectFlag = false;

      canvas->toolPanel->myReset();
      redrawToolBar();
      return;
    }
  }


  if(e->y()>=DRAWMODEICON_Y && e->y()<DRAWMODEICON_Y+DRAWMODEICON_HEIGHT){
    if(e->x()>=DRAWMODEICON_X && e->x()<DRAWMODEICON_X+DRAWMODEICON_WIDTH){
      if(canvas->drawModePanel->isVisible()){
	canvas->drawModePanel->hide();
      }
      else{
	canvas->drawModePanel->show();
      }
      canvas->toolbar->redrawToolBar();
      return;
    }
  }


  if(e->y()>=PENSIZEICON_Y && e->y()<PENSIZEICON_Y+PENSIZEICON_HEIGHT){
    if(e->x()>=PENSIZEICON_X && e->x()<PENSIZEICON_X+PENSIZEICON_WIDTH){

      if(canvas->pensizePanel->isVisible()){
	canvas->pensizePanel->hide();
	canvas->pensizePanel->display = false;
      }
      else{
	canvas->pensizePanel->show();
	canvas->pensizePanel->display = true;
      }
      canvas->toolbar->redrawToolBar();
      return;
    }
  }

  if(e->y()>=LAYERICON_Y && e->y()<LAYERICON_Y+LAYERICON_HEIGHT){
    if(e->x()>=LAYERICON_X && e->x()<LAYERICON_X+LAYERICON_WIDTH){

      if(canvas->layerPanel->isVisible()){
	canvas->layerPanel->hide();
	canvas->layerPanel->display = false;
      }
      else{
	canvas->layerPanel->show();
	canvas->layerPanel->display = true;
      }
      canvas->toolbar->redrawToolBar();
      return;
    }
  }


  if(e->y()>=MENUICON_Y && e->y()<MENUICON_Y+MENUICON_HEIGHT){
    if(e->x()>=MENUICON_X && e->x()<MENUICON_X+MENUICON_WIDTH){
      if(canvas->fileMenuPanel->isVisible()){
	canvas->fileMenuPanel->hide();

      for(int y=FILEMENUICON_Y; y<FILEMENUICON_Y+FILEMENUICON_HEIGHT; y++){
	for(int x=FILEMENUICON_X; x<FILEMENUICON_X+FILEMENUICON_WIDTH; x++){
	  canvas->toolbar->toolbarImg.setPixel(x,y,canvas->toolbar->toolbar_normalImg.pixel(x,y));
	}
      }
      }
      else{
	canvas->fileMenuPanel->show();
      for(int y=FILEMENUICON_Y; y<FILEMENUICON_Y+FILEMENUICON_HEIGHT; y++){
	for(int x=FILEMENUICON_X; x<FILEMENUICON_X+FILEMENUICON_WIDTH; x++){
	  canvas->toolbar->toolbarImg.setPixel(x,y,canvas->toolbar->toolbar_reverseImg.pixel(x,y));
	}
      }
      }

      redrawToolBar();

      return;
    }
  }


  if(e->y()>=MESHICON_Y && e->y()<MESHICON_Y+MESHICON_HEIGHT){
    if(e->x()>=MESHICON_X && e->x()<MESHICON_X+MESHICON_WIDTH){
      
      if(canvas->meshPanel->isVisible()){
	canvas->meshPanel->hide();
	canvas->meshPanel->display = false;
      }
      else{
	canvas->meshPanel->show();
	canvas->meshPanel->display = true;
      }
      canvas->toolbar->redrawToolBar();
      return;
    }
  }

  if(e->y()>=COLORAREA3_Y && e->y()<COLORAREA3_Y+COLORAREA3_HEIGHT){
    if(e->x()>=COLORAREA3_X && e->x()<COLORAREA3_X+COLORAREA3_WIDTH){
      if(canvas->palettePanel->isVisible()){
	canvas->palettePanel->hide();
	canvas->palettePanel->display = false;
      }
      else{
	canvas->palettePanel->show();
	canvas->palettePanel->display = true;
      }

      return;
    }
  }



  if(canvas->zoomStyle != LUPE){
    if(e->y()>=HANDICON_Y && e->y()<HANDICON_Y+HANDICON_HEIGHT){
      if(e->x()>=HANDICON_X && e->x()<HANDICON_X+HANDICON_WIDTH){
	canvas->handFlag = !canvas->handFlag;
	canvas->spoitSelectFlag = false;
	redrawToolBar();
	return;
      }
    }
  }

  if(e->y()>=SCALEICON_Y && e->y()<SCALEICON_Y+SCALEICON_HEIGHT){
    if(e->x()>=SCALEICON_X && e->x()<SCALEICON_X+SCALEICON_WIDTH){
      if(canvas->scalePanel->isVisible()){
	canvas->scalePanel->hide();
	canvas->scalePanel->display = false;
	canvas->zoomSelectFlag = false;
	canvas->toolbar->redrawToolBar();
	return;
      }


      if(canvas->zoomStyle == NORMAL || canvas->zoomStyle == ZOOM){
	if(canvas->zoomScale == 0){
	  canvas->zoomSelectFlag = true;
	}
	if(canvas->canvasType == QVGA && canvas->zoomScale == 1){
	  canvas->zoomSelectFlag = true;
	}

	canvas->scalePanel->show();
	canvas->scalePanel->display = true;
      }
      else if(canvas->zoomStyle == LUPE){
	if(canvas->canvasType == VGA){
	  canvas->zoomStyle = NORMAL;
	}
	else if(canvas->canvasType == QVGA){
	  canvas->zoomStyle = ZOOM;
	}
	canvas->lupePanel->hide();

	if(canvas->zoomScale == 0){
	  canvas->zoomSelectFlag = true;
	}
	canvas->scalePanel->show();
	canvas->scalePanel->display = true;
      }
      canvas->toolbar->redrawToolBar();
      return;
    }

  }

  if(e->y()>=TOOLICON_Y && e->y()<TOOLICON_Y+TOOLICON_HEIGHT){
    if(e->x()>=TOOLICON_X && e->x()<TOOLICON_X+TOOLICON_WIDTH){
      if(canvas->toolPanel->isVisible()){
	canvas->toolPanel->hide();
	canvas->toolPanel->display = false;
	canvas->toolbar->redrawToolBar();
	return;
      }
      else{
	canvas->toolPanel->show();
	canvas->toolPanel->display = true;
	canvas->toolbar->redrawToolBar();
	return;
      }
    }

  }


  if(e->y()>=LUPEICON_Y && e->y()<LUPEICON_Y+LUPEICON_HEIGHT){
    if(e->x()>=LUPEICON_X && e->x()<LUPEICON_X+LUPEICON_WIDTH){
      if(canvas->zoomStyle == LUPE){
	if(canvas->canvasType == VGA){
	  canvas->zoomStyle = NORMAL;
	}
	else if(canvas->canvasType == QVGA){
	  canvas->zoomStyle = ZOOM;
	}
	canvas->lupePanel->hide();

	//	canvas->repaint();
      }
      else if(canvas->zoomStyle == NORMAL){
	canvas->zoomStyle = LUPE;
	canvas->lupePanel->show();

	canvas->handFlag = false;	

	canvas->selectPanel->hide();
	canvas->selectPanel->display = false;
	if(canvas->moveRectFlag){
	  for(int y=0; y<640; y++){
	    for(int x=0; x<480; x++){
	      *(canvas->undoBuffer+x+y*480) = canvas->layer[canvas->currentlayer]->pixel(x,y);
	    }
	  }
	  canvas->moveRectFlag = false;
	}
	canvas->undoFlag = false;
	canvas->toolbar->redrawToolBar();


	canvas->canvasTop = 0;
	canvas->canvasLeft = 0;

	canvas->repaint();
      }
      else if(canvas->zoomStyle == ZOOM){

	if(canvas->canvasType == VGA && canvas->zoomScale > 0)
	  canvas->lastScale = canvas->zoomScale;
	else if(canvas->canvasType == QVGA && canvas->zoomScale > 1)
	  canvas->lastScale = canvas->zoomScale;


	if(canvas->canvasType == VGA){
	  canvas->zoomStyle = NORMAL;
	  canvas->zoomScale = 0;
	}
	else if(canvas->canvasType == QVGA){
	  canvas->zoomScale = 1;
	  canvas->zoomTop = 0;
	  canvas->zoomLeft = 0;
	}

	canvas->scalePanel->changeScale(0, 0);
	canvas->handFlag = false;

	canvas->canvasTop = 0;
	canvas->canvasLeft = 0;
	
	canvas->repaint();

	canvas->zoomStyle = LUPE;

	canvas->selectPanel->hide();
	canvas->selectPanel->display = false;
	if(canvas->moveRectFlag){
	  for(int y=0; y<640; y++){
	    for(int x=0; x<480; x++){
	      *(canvas->undoBuffer+x+y*480) = canvas->layer[canvas->currentlayer]->pixel(x,y);
	    }
	  }
	  canvas->moveRectFlag = false;
	}
	canvas->undoFlag = false;
	canvas->toolbar->redrawToolBar();


	canvas->lupePanel->show();


      }

      if(canvas->scalePanel->isVisible()){
	canvas->scalePanel->myReset();
      }

      canvas->toolbar->redrawToolBar();
      return;
    }
  }

  if(canvas->zoomStyle != LUPE){
    if(e->y()>=SELECTICON_Y && e->y()<SELECTICON_Y+SELECTICON_HEIGHT){
      if(e->x()>=SELECTICON_X && e->x()<SELECTICON_X+SELECTICON_WIDTH){
	if(canvas->selectPanel->isVisible()){
	  canvas->selectPanel->hide();
	  canvas->selectPanel->display = false;
	}
	else{
	  canvas->selectPanel->show();
	  canvas->selectPanel->display = true;
	}
	canvas->toolbar->redrawToolBar();
	return;
      }
    }
  }


  if(e->y()>=SPOIT_Y && e->y()<SPOIT_Y+SPOIT_HEIGHT){
    if(e->x()>=SPOIT_X && e->x()<SPOIT_X+SPOIT_WIDTH){
      canvas->spoitSelectFlag = !canvas->spoitSelectFlag;
      canvas->handFlag = false;

      canvas->toolbar->redrawToolBar();

      return;

    }
  }

  if(e->y()>=UNDO_Y && e->y()<UNDO_Y+UNDO_HEIGHT){
    if(e->x()>=UNDO_X && e->x()<UNDO_X+UNDO_WIDTH){
      if(canvas->undoFlag){
	// redo
	canvas->undoFlag = false;
	for(int y=0; y<640; y++){
	  for(int x=0; x<480; x++){
	    *(canvas->undoBuffer+x+y*480) = canvas->layer[canvas->currentlayer]->pixel(x,y);
	    canvas->layer[canvas->currentlayer]->setPixel(x,y, *(canvas->redoBuffer+x+y*480));
	  }
	}

	for(int y=0; y<640; y++){
	  for(int x=0; x<480; x++){
	    *(canvas->redoBuffer+x+y*480) = 0x00ffffff;
	  }
	}

	canvas->repaint();
	if(canvas->lupePanel->isVisible()){
	  canvas->lupePanel->myReset();
	}
      }
      else{
	// undo
	canvas->undoFlag = true;
	canvas->moveRectFlag = false;
	canvas->selectPanel->myReset();

	for(int y=0; y<640; y++){
	  for(int x=0; x<480; x++){
	    *(canvas->redoBuffer+x+y*480) = canvas->layer[canvas->currentlayer]->pixel(x,y);
	    canvas->layer[canvas->currentlayer]->setPixel(x,y, *(canvas->undoBuffer+x+y*480));
	  }
	}
	canvas->repaint();
      }
      
      canvas->toolbar->redrawToolBar();
      if(canvas->lupePanel->isVisible()){
	canvas->lupePanel->myReset();
      }

      return;

    }
  }


  if(!opacitySelectingFlag && e->y()>=SBAREA_Y && e->y()< SBAREA_Y+SBAREA_HEIGHT){
    if(e->x()>=SBAREA_X && e->x()< SBAREA_X+SBAREA_WIDTH){

      sbSelectingFlag =true; // OpacityAreaを反応させないためのフラグ

      reverseSBCursor();

      canvas->fgSa = (e->x()-SBAREA_X)*COLORDIV/169;
      canvas->fgBr = (57-e->y()+SBAREA_Y)*COLORDIV/57;
      canvas->fgSa2 = ((double)(e->x()-SBAREA_X))/169.0;
      canvas->fgBr2 = ((double)(57-e->y()+SBAREA_Y))/57.0;
      canvas->HSB2RGBd();

      changeSBArea();

      changeColorArea();

      //ツールバー画像の表示
      //      QPixmap toolbarPix = QPixmap();
      //      toolbarPix.convertFromImage(toolbarImg, 0);
      //      setBackgroundPixmap(toolbarPix);

  repaint_left = SBAREA_X-2;
  repaint_top = SBAREA_Y-2;
  repaint_width = SBAREA_WIDTH+4;
  repaint_height = SBAREA_HEIGHT+4;
  QRegion r(repaint_left,repaint_top,repaint_width,repaint_height,QRegion::Rectangle);
  repaint(r, false);

  repaint_left = COLORAREA_X;
  repaint_top = COLORAREA_Y;
  repaint_width = COLORAREA_WIDTH;
  repaint_height = COLORAREA_HEIGHT;
  r = QRegion(repaint_left,repaint_top,repaint_width,repaint_height,QRegion::Rectangle);
  repaint(r, false);

  repaint_left = COLORAREA3_X;
  repaint_top = COLORAREA3_Y;
  repaint_width = COLORAREA3_WIDTH;
  repaint_height = COLORAREA3_HEIGHT;
  r = QRegion(repaint_left,repaint_top,repaint_width,repaint_height,QRegion::Rectangle);
  repaint(r, false);

      return;

    }
  }


  if(!sbSelectingFlag && e->y()>=OPACITY_Y-6 && e->y()< OPACITY_Y+OPACITY_HEIGHT){
    if(e->x()>=OPACITY_X-1 && e->x()< OPACITY_X+OPACITY_WIDTH+2){
      
      opacitySelectingFlag = true; // SBAreaを反応させないためのフラグ

      if(canvas->tool == ERASER){
	tmpAlpha = canvas->eraseralpha;
      }
      else if(canvas->tool == PEN){
	tmpAlpha = canvas->penalpha;
      }
      else if(canvas->tool == BLUR){
	tmpAlpha = canvas->bluralpha;
      }
      else if(canvas->tool == SHARPEN){
	tmpAlpha = canvas->sharpenalpha;
      }

      toolbarImg.setPixel(OPACITY_X+1+tmpAlpha,OPACITY_Y+1, 0xff9B9B9B);
      toolbarImg.setPixel(OPACITY_X+1+tmpAlpha,OPACITY_Y+2, 0xff9B9B9B);
      toolbarImg.setPixel(OPACITY_X+1+tmpAlpha,OPACITY_Y+3, 0xff9B9B9B);
      toolbarImg.setPixel(OPACITY_X+1+tmpAlpha,OPACITY_Y+4, 0xff9B9B9B);

      toolbarImg.setPixel(OPACITY_X  +tmpAlpha,OPACITY_Y+2, 0xff9B9B9B);
      toolbarImg.setPixel(OPACITY_X  +tmpAlpha,OPACITY_Y+3, 0xff9B9B9B);
      toolbarImg.setPixel(OPACITY_X  +tmpAlpha,OPACITY_Y+4, 0xff9B9B9B);

      toolbarImg.setPixel(OPACITY_X+2+tmpAlpha,OPACITY_Y+2, 0xff9B9B9B);
      toolbarImg.setPixel(OPACITY_X+2+tmpAlpha,OPACITY_Y+3, 0xff9B9B9B);
      toolbarImg.setPixel(OPACITY_X+2+tmpAlpha,OPACITY_Y+4, 0xff9B9B9B);

      changeOpacityArea(e->x());
      changeColorArea();

      for(int x=OPACITY_X+75; x<OPACITY_X+OPACITY_WIDTH-7;x++)
	for(int y=OPACITY_Y-11; y<OPACITY_Y;y++)
	  toolbarImg.setPixel(x,y,0xff9B9B9B);

      //ツールバー画像の表示
      QPixmap toolbarPix = QPixmap();
      toolbarPix.convertFromImage(toolbarImg, 0);
      
      if(canvas->tool == ERASER){
	tmpStr = tmpStr.sprintf("%d",canvas->eraseralpha);
      }
      else if(canvas->tool == PEN){
	tmpStr = tmpStr.sprintf("%d",canvas->penalpha);
      }
      else if(canvas->tool == BLUR){
	tmpStr = tmpStr.sprintf("%d",canvas->bluralpha);
      }
      else if(canvas->tool == SHARPEN){
	tmpStr = tmpStr.sprintf("%d",canvas->sharpenalpha);
      }

      QPainter p;
      p.begin( &toolbarPix );
      p.setPen( black );
      p.setFont(QFont("lcfont", 8));
      p.drawText(QRect(OPACITY_X,OPACITY_Y-11, OPACITY_WIDTH-7, 12),Qt::AlignRight , tmpStr);
      p.end();  

            toolbarImg = toolbarPix.convertToImage();

      //      setBackgroundPixmap(toolbarPix);
  repaint_left = OPACITY_X-2;
  repaint_top = OPACITY_Y-12;
  repaint_width = OPACITY_WIDTH+5;
  repaint_height = OPACITY_HEIGHT+12;
  QRegion r(repaint_left,repaint_top,repaint_width,repaint_height,QRegion::Rectangle);
  repaint(r, false);

  repaint_left = COLORAREA3_X;
  repaint_top = COLORAREA3_Y;
  repaint_width = COLORAREA3_WIDTH;
  repaint_height = COLORAREA3_HEIGHT;
  r = QRegion(repaint_left,repaint_top,repaint_width,repaint_height,QRegion::Rectangle);
  repaint(r, false);

      return;
    }
  }

  if(!opacitySelectingFlag){
  if(e->y()>=OPACITY_MINUS_Y && e->y()<OPACITY_MINUS_Y+OPACITY_MINUS_HEIGHT){
    if(e->x()>=OPACITY_MINUS_X && e->x()<OPACITY_MINUS_X+OPACITY_MINUS_WIDTH){
      opacityMinusSelectFlag = true;
      canvas->toolbar->redrawToolBar();
      return;
    }
  }

  if(e->y()>=OPACITY_PLUS_Y && e->y()<OPACITY_PLUS_Y+OPACITY_PLUS_HEIGHT){
    if(e->x()>=OPACITY_PLUS_X && e->x()<OPACITY_PLUS_X+OPACITY_PLUS_WIDTH){
      opacityPlusSelectFlag = true;
      canvas->toolbar->redrawToolBar();
      return;
    }
  }
  }
}


void MyToolBar::mouseMoveEvent( QMouseEvent *e )
{
  QPainter p;
  QString tmpStr = "   ";
  int tmpSa, tmpBr, tmpOpacity;
  int tmpAlpha = 0;

  opacityMinusSelectFlag = false;
  opacityPlusSelectFlag = false;


  if(sbSelectingFlag){
    if(e->y()<SBAREA_Y){
      tmpBr = SBAREA_Y;
    }
    else if(e->y()>= SBAREA_Y+SBAREA_HEIGHT){
      tmpBr = SBAREA_Y+SBAREA_HEIGHT-1;
    }
    else{
      tmpBr = e->y();
    }

    if(e->x()<SBAREA_X){
      tmpSa = SBAREA_X;
    }
    else if(e->x()>= SBAREA_X+SBAREA_WIDTH){
      tmpSa = SBAREA_X+SBAREA_WIDTH-1;
    }
    else{
      tmpSa = e->x();
    }

      reverseSBCursor();

      canvas->fgSa = (tmpSa-SBAREA_X)*COLORDIV/169;
      canvas->fgBr = (57-tmpBr+SBAREA_Y)*COLORDIV/57;
      canvas->fgSa2 = ((double)(tmpSa-SBAREA_X))/169.0;
      canvas->fgBr2 = ((double)(57-tmpBr+SBAREA_Y))/57.0;
      canvas->HSB2RGBd();

      changeSBArea(); 
      changeColorArea();

      //ツールバー画像の表示
      //      QPixmap toolbarPix = QPixmap();
      //      toolbarPix.convertFromImage(toolbarImg, 0);
      //      setBackgroundPixmap(toolbarPix);

  repaint_left = SBAREA_X-2;
  repaint_top = SBAREA_Y-2;
  repaint_width = SBAREA_WIDTH+4;
  repaint_height = SBAREA_HEIGHT+4;
  QRegion r(repaint_left,repaint_top,repaint_width,repaint_height,QRegion::Rectangle);
  repaint(r, false);

  repaint_left = COLORAREA_X;
  repaint_top = COLORAREA_Y;
  repaint_width = COLORAREA_WIDTH;
  repaint_height = COLORAREA_HEIGHT;
  r = QRegion(repaint_left,repaint_top,repaint_width,repaint_height,QRegion::Rectangle);
  repaint(r, false);

  repaint_left = COLORAREA3_X;
  repaint_top = COLORAREA3_Y;
  repaint_width = COLORAREA3_WIDTH;
  repaint_height = COLORAREA3_HEIGHT;
  r = QRegion(repaint_left,repaint_top,repaint_width,repaint_height,QRegion::Rectangle);
  repaint(r, false);

  return;
  }



  if(opacitySelectingFlag){
    if(e->x()<OPACITY_X-1){
	tmpOpacity = OPACITY_X-1;
    }
    else if(e->x()>=OPACITY_X+OPACITY_WIDTH+2){
	tmpOpacity = OPACITY_X+OPACITY_WIDTH+1;
    }
    else{
	tmpOpacity = e->x();
    }


    if(canvas->tool == ERASER){
      tmpAlpha = canvas->eraseralpha;
    }
    else if(canvas->tool == PEN){
      tmpAlpha = canvas->penalpha;
    }
    else if(canvas->tool == BLUR){
      tmpAlpha = canvas->bluralpha;
    }
    else if(canvas->tool == SHARPEN){
      tmpAlpha = canvas->sharpenalpha;
    }

      toolbarImg.setPixel(OPACITY_X+1+tmpAlpha,OPACITY_Y+1, 0xff9B9B9B);
      toolbarImg.setPixel(OPACITY_X+1+tmpAlpha,OPACITY_Y+2, 0xff9B9B9B);
      toolbarImg.setPixel(OPACITY_X+1+tmpAlpha,OPACITY_Y+3, 0xff9B9B9B);
      toolbarImg.setPixel(OPACITY_X+1+tmpAlpha,OPACITY_Y+4, 0xff9B9B9B);

      toolbarImg.setPixel(OPACITY_X  +tmpAlpha,OPACITY_Y+2, 0xff9B9B9B);
      toolbarImg.setPixel(OPACITY_X  +tmpAlpha,OPACITY_Y+3, 0xff9B9B9B);
      toolbarImg.setPixel(OPACITY_X  +tmpAlpha,OPACITY_Y+4, 0xff9B9B9B);

      toolbarImg.setPixel(OPACITY_X+2+tmpAlpha,OPACITY_Y+2, 0xff9B9B9B);
      toolbarImg.setPixel(OPACITY_X+2+tmpAlpha,OPACITY_Y+3, 0xff9B9B9B);
      toolbarImg.setPixel(OPACITY_X+2+tmpAlpha,OPACITY_Y+4, 0xff9B9B9B);

      changeOpacityArea(tmpOpacity);
      changeColorArea();

      for(int x=OPACITY_X+75; x<OPACITY_X+OPACITY_WIDTH-7;x++)
	for(int y=OPACITY_Y-11; y<OPACITY_Y;y++)
	  toolbarImg.setPixel(x,y,0xff9B9B9B);

      //ツールバー画像の表示
      QPixmap toolbarPix = QPixmap();
      toolbarPix.convertFromImage(toolbarImg, 0);
      

      if(canvas->tool == ERASER){
	tmpStr = tmpStr.sprintf("%d",canvas->eraseralpha);
      }
      else if(canvas->tool == PEN){
	tmpStr = tmpStr.sprintf("%d",canvas->penalpha);
      }
      else if(canvas->tool == BLUR){
	tmpStr = tmpStr.sprintf("%d",canvas->bluralpha);
      }
      else if(canvas->tool == SHARPEN){
	tmpStr = tmpStr.sprintf("%d",canvas->sharpenalpha);
      }

      QPainter p;
      p.begin( &toolbarPix );
      p.setPen( black );
      p.setFont(QFont("lcfont", 8));
      p.drawText(QRect(OPACITY_X,OPACITY_Y-11, OPACITY_WIDTH-7, 12),Qt::AlignRight , tmpStr);
      p.end();  

            toolbarImg = toolbarPix.convertToImage();

      //      setBackgroundPixmap(toolbarPix);
  repaint_left = OPACITY_X-2;
  repaint_top = OPACITY_Y-12;
  repaint_width = OPACITY_WIDTH+5;
  repaint_height = OPACITY_HEIGHT+12;
  QRegion r(repaint_left,repaint_top,repaint_width,repaint_height,QRegion::Rectangle);
  repaint(r, false);

  repaint_left = COLORAREA3_X;
  repaint_top = COLORAREA3_Y;
  repaint_width = COLORAREA3_WIDTH;
  repaint_height = COLORAREA3_HEIGHT;
  r = QRegion(repaint_left,repaint_top,repaint_width,repaint_height,QRegion::Rectangle);
  repaint(r, false);

  }


  if(!opacitySelectingFlag){
  if(e->y()>=OPACITY_MINUS_Y && e->y()<OPACITY_MINUS_Y+OPACITY_MINUS_HEIGHT){
    if(e->x()>=OPACITY_MINUS_X && e->x()<OPACITY_MINUS_X+OPACITY_MINUS_WIDTH){
      opacityMinusSelectFlag = true;
      canvas->toolbar->redrawToolBar();
      return;
    }
  }

  if(e->y()>=OPACITY_PLUS_Y && e->y()<OPACITY_PLUS_Y+OPACITY_PLUS_HEIGHT){
    if(e->x()>=OPACITY_PLUS_X && e->x()<OPACITY_PLUS_X+OPACITY_PLUS_WIDTH){
      opacityPlusSelectFlag = true;
      canvas->toolbar->redrawToolBar();
      return;
    }
  }
  }
}


void MyToolBar::mouseReleaseEvent( QMouseEvent *e )
{
  QPainter p;
  int tmpOpacity = 0;
  int tmpSa = 0;
  int tmpBr = 0;

  opacityMinusSelectFlag = false;
  opacityPlusSelectFlag = false;


  if(sbSelectingFlag){
    sbSelectingFlag = false;
    if(e->y()<SBAREA_Y){
      tmpBr = SBAREA_Y;
    }
    else if(e->y()>= SBAREA_Y+SBAREA_HEIGHT){
      tmpBr = SBAREA_Y+SBAREA_HEIGHT-1;
    }
    else{
      tmpBr = e->y();
    }

    if(e->x()<SBAREA_X){
      tmpSa = SBAREA_X;
    }
    else if(e->x()>= SBAREA_X+SBAREA_WIDTH){
      tmpSa = SBAREA_X+SBAREA_WIDTH-1;
    }
    else{
      tmpSa = e->x();
    }

    reverseSBCursor();

    canvas->fgSa = (tmpSa-SBAREA_X)*COLORDIV/169;
    canvas->fgBr = (57-tmpBr+SBAREA_Y)*COLORDIV/57;
    canvas->fgSa2 = ((double)(tmpSa-SBAREA_X))/169;
    canvas->fgBr2 = ((double)(57-tmpBr+SBAREA_Y))/57;
    canvas->HSB2RGBd();

    changeSBArea(); 
    changeColorArea();

    //ツールバー画像の表示
    //      QPixmap toolbarPix = QPixmap();
    //      toolbarPix.convertFromImage(toolbarImg, 0);
    //      setBackgroundPixmap(toolbarPix);

    repaint_left = SBAREA_X-2;
    repaint_top = SBAREA_Y-2;
    repaint_width = SBAREA_WIDTH+4;
    repaint_height = SBAREA_HEIGHT+4;
    QRegion r(repaint_left,repaint_top,repaint_width,repaint_height,QRegion::Rectangle);
    repaint(r, false);

    repaint_left = COLORAREA_X;
    repaint_top = COLORAREA_Y;
    repaint_width = COLORAREA_WIDTH;
    repaint_height = COLORAREA_HEIGHT;
    r = QRegion(repaint_left,repaint_top,repaint_width,repaint_height,QRegion::Rectangle);
    repaint(r, false);

    repaint_left = COLORAREA3_X;
    repaint_top = COLORAREA3_Y;
    repaint_width = COLORAREA3_WIDTH;
    repaint_height = COLORAREA3_HEIGHT;
    r = QRegion(repaint_left,repaint_top,repaint_width,repaint_height,QRegion::Rectangle);
    repaint(r, false);

    return;

  }


  if(opacitySelectingFlag){
    opacitySelectingFlag = false;

    if(e->x()<OPACITY_X-1){
	tmpOpacity = OPACITY_X-1;
    }
    else if(e->x()>=OPACITY_X+OPACITY_WIDTH+2){
	tmpOpacity = OPACITY_X+OPACITY_WIDTH+1;
    }
    else{
	tmpOpacity = e->x();
    }

    changeOpacityArea2(tmpOpacity);

    repaint_left = OPACITY_X-2;
    repaint_top = OPACITY_Y-12;
    repaint_width = OPACITY_WIDTH+5;
    repaint_height = OPACITY_HEIGHT+12;
    QRegion r(repaint_left,repaint_top,repaint_width,repaint_height,QRegion::Rectangle);
    repaint(r, false);

    repaint_left = COLORAREA3_X;
    repaint_top = COLORAREA3_Y;
    repaint_width = COLORAREA3_WIDTH;
    repaint_height = COLORAREA3_HEIGHT;
    r = QRegion(repaint_left,repaint_top,repaint_width,repaint_height,QRegion::Rectangle);
    repaint(r, false);

    //    canvas->toolbar->redrawToolBar();
    return;
 
  }


  if(e->y()>=OPACITY_MINUS_Y && e->y()<OPACITY_MINUS_Y+OPACITY_MINUS_HEIGHT){
    if(e->x()>=OPACITY_MINUS_X && e->x()<OPACITY_MINUS_X+OPACITY_MINUS_WIDTH){

      if(canvas->tool == ERASER){
	tmpOpacity = OPACITY_X + canvas->eraseralpha -1 ;
      }
      else if(canvas->tool == PEN){
	tmpOpacity = OPACITY_X + canvas->penalpha - 1;
      }
      else if(canvas->tool == BLUR){
	tmpOpacity = OPACITY_X + canvas->bluralpha - 1;
      }
      else if(canvas->tool == SHARPEN){
	tmpOpacity = OPACITY_X + canvas->sharpenalpha - 1;
      }

      if(tmpOpacity< OPACITY_X + 0){
	tmpOpacity = OPACITY_X + 0;
      }
      else if(tmpOpacity > OPACITY_X + 100){
	tmpOpacity = OPACITY_X + 100;
      }
      changeOpacityArea2(tmpOpacity);
      canvas->toolbar->redrawToolBar();      
      return;
    }
  }

  if(e->y()>=OPACITY_PLUS_Y && e->y()<OPACITY_PLUS_Y+OPACITY_PLUS_HEIGHT){
    if(e->x()>=OPACITY_PLUS_X && e->x()<OPACITY_PLUS_X+OPACITY_PLUS_WIDTH){

      if(canvas->tool == ERASER){
	tmpOpacity = OPACITY_X + canvas->eraseralpha +1 ;
      }
      else if(canvas->tool == PEN){
	tmpOpacity = OPACITY_X + canvas->penalpha + 1;
      }
      else if(canvas->tool == BLUR){
	tmpOpacity = OPACITY_X + canvas->bluralpha + 1;
      }
      else if(canvas->tool == SHARPEN){
	tmpOpacity = OPACITY_X + canvas->sharpenalpha + 1;
      }


      if(tmpOpacity< OPACITY_X + 0){
	tmpOpacity = OPACITY_X + 0;
      }
      else if(tmpOpacity > OPACITY_X + 100){
	tmpOpacity = OPACITY_X + 100;
      }
      changeOpacityArea2(tmpOpacity);
      canvas->toolbar->redrawToolBar();      
      return;
    }
  }

  canvas->toolbar->redrawToolBar();
  return;

}

