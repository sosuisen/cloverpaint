#include <unistd.h>

#include "clover.h"


#include "layer_hogo.h"
#include "layer_free.h"
#include "layer_bottom_normal.h"
#include "layer_bottom_reverse.h"
#include "layer_bottom_background.h"
#include "layer_top_normal.h"
#include "layer_top_reverse.h"
#include "layer_normal.h"
#include "layer_nomode.h"
#include "layer_visible.h"
#include "layer_invisible.h"

#include <qmessagebox.h>

#define OPACITY_X 21
#define OPACITY_Y 33
#define OPACITY_WIDTH 100
#define OPACITY_HEIGHT 9

#define MESHICON_X 331
#define MESHICON_Y 2
#define MESHICON_WIDTH 29
#define MESHICON_HEIGHT 29


Layer::Layer(Canvas * parent, const char * name, bool modal, WFlags f)
  :QDialog(parent, name, modal, f)
{
  canvas = parent;

  moveFlag = false;
  stickyFlag = false;
  minimizePressFlag = false;
  closePressFlag = false;
  display = false;

  reparent( parent, QPoint(LAYERPANEL_X, canvas->layerPanelY));

  resize(148, 45+20+29*canvas->layernum);

  layerPanelImg = new QImage (148, 45+20+29*canvas->layernum, 32, 0, QImage::IgnoreEndian );

  //画像の読み込み
  QImage *tmpImg = new QImage(layer_normal);
  //xpmからの読みこみはインデックスカラーになるため変換
  layer_normalImg = new QImage(tmpImg->convertDepth(32));
  layer_normalImgOrg = new QImage(tmpImg->convertDepth(32));
  delete tmpImg;

  tmpImg = new QImage(layer_hogo);
  layer_hogoImg = new QImage(tmpImg->convertDepth(32));
  delete tmpImg;

  tmpImg = new QImage(layer_free);
  layer_freeImg = new QImage(tmpImg->convertDepth(32));
  delete tmpImg;

  tmpImg = new QImage(layer_nomode);
  layer_nomodeImg = new QImage(tmpImg->convertDepth(32));
  delete tmpImg;

  tmpImg = new QImage(layer_visible);
  layer_visibleImg = new QImage(tmpImg->convertDepth(32));
  delete tmpImg;

  tmpImg = new QImage(layer_invisible);
  layer_invisibleImg = new QImage(tmpImg->convertDepth(32));
  delete tmpImg;

  tmpImg = new QImage(layer_top_normal);
  layer_top_normalImg = new QImage(tmpImg->convertDepth(32));
  delete tmpImg;

  tmpImg = new QImage(layer_top_reverse);
  layer_top_reverseImg = new QImage(tmpImg->convertDepth(32));
  delete tmpImg;

  tmpImg = new QImage(layer_bottom_normal);
  layer_bottom_normalImg = new QImage(tmpImg->convertDepth(32));
  delete tmpImg;

  tmpImg = new QImage(layer_bottom_reverse);
  layer_bottom_reverseImg = new QImage(tmpImg->convertDepth(32));
  delete tmpImg;

  tmpImg = new QImage(layer_bottom_background);
  layer_bottom_backgroundImg = new QImage(tmpImg->convertDepth(32));
  delete tmpImg;

  for(int y=0; y<20; y++){
    for(int x=0; x<148; x++){
      layerPanelImg->setPixel(x,y,layer_top_normalImg->pixel(x,y));
    }
  }

  for(int i=0; i<canvas->layernum; i++){
    if(canvas->currentlayer != 0){
      for(int y=0; y<21; y++){
	for(int x=0; x<62; x++){
	  layer_normalImg->setPixel(x+82,y+4,canvas->mode_normalImg->pixel(x+1,y+1+canvas->layerMode[canvas->layernum-i-1]*21));
	}
      }
    }
    else{
      for(int y=0; y<21; y++){
	for(int x=0; x<62; x++){
	  layer_normalImg->setPixel(x+82,y+4,layer_nomodeImg->pixel(x,y));
	}
      }
    }

    if(canvas->layerVisible[canvas->layernum-i-1]){
      for(int y=0; y<29; y++){
	for(int x=0; x<18; x++){
	  layer_normalImg->setPixel(x,y,layer_visibleImg->pixel(x,y));
	}
      }
    }
    else{
      for(int y=0; y<29; y++){
	for(int x=0; x<18; x++){
	  layer_normalImg->setPixel(x,y,layer_invisibleImg->pixel(x,y));
	}
      }
    }

    if(canvas->layernum-i-1 == 0){
      for(int y=0; y<29; y++){
	for(int x=19; x<34; x++){
	  layer_normalImg->setPixel(x,y,layer_normalImgOrg->pixel(x,y));
	}
      }
    }
    else if(canvas->layerDrawable[canvas->layernum-i-1]){
      for(int y=0; y<29; y++){
	for(int x=19; x<34; x++){
	  layer_normalImg->setPixel(x,y,layer_freeImg->pixel(x-19,y));
	}
      }
    }
    else{
      for(int y=0; y<29; y++){
	for(int x=19; x<34; x++){
	  layer_normalImg->setPixel(x,y,layer_hogoImg->pixel(x-19,y));
	}
      }
    }

    for(int y=0; y<29; y++){
      for(int x=0; x<148; x++){
	layerPanelImg->setPixel(x,y+20+i*29,layer_normalImg->pixel(x,y));
      }
    }

  }


  if(canvas->currentlayer != 0){
    for(int y=0; y<21; y++){
      for(int x=0; x<62; x++){
	layer_normalImg->setPixel(x+82,y+4,canvas->mode_normalImg->pixel(x+1,y+1+canvas->layerMode[canvas->currentlayer]*21));
      }
    }
  }



  for(int y=0; y<29; y++){
    for(int x=1; x<1+145; x++){
      if(x >= 82 && x <=143 && y>=4 && y <=24 && canvas->currentlayer != 0){
	layerPanelImg->setPixel(x,y+20+(canvas->layernum-canvas->currentlayer-1)*29,layer_normalImg->pixel(x,y));
      }
      else{
	layerPanelImg->setPixel(x,y+20+(canvas->layernum-canvas->currentlayer-1)*29,(~(layer_normalImg->pixel(x,y))) | 0xff000000);
      }
    }
  }

  for(int y=0; y<45; y++){
    for(int x=0; x<148; x++){
      layerPanelImg->setPixel(x,y+20+canvas->layernum*29,layer_bottom_normalImg->pixel(x,y));
    }
  }
  if(canvas->currentlayer == 0){
    if(canvas->layernum <= 1){
      for(int y=0; y<22; y++){
	for(int x=69; x< 148; x++){
	  layerPanelImg->setPixel(x, 20+29*canvas->layernum + y, layer_bottom_backgroundImg->pixel(x,y));
	}
      }
    }
    else{
      for(int y=0; y<22; y++){
	for(int x=90; x< 148; x++){
	  layerPanelImg->setPixel(x, 20+29*canvas->layernum + y, layer_bottom_backgroundImg->pixel(x,y));
	}
      }
    }
    for(int y=22; y<41; y++){
      for(int x=0; x< 148; x++){
	layerPanelImg->setPixel(x, 20+29*canvas->layernum + y, layer_bottom_backgroundImg->pixel(x,y));
      }
    }

  }
  if(canvas->currentlayer == 1){
    for(int y=0; y<22; y++){
      for(int x=128; x< 148; x++){
	layerPanelImg->setPixel(x, 20+29*canvas->layernum + y, layer_bottom_backgroundImg->pixel(x,y));
      }
    }
  }
  if(canvas->currentlayer == canvas->layernum -1){
    for(int y=0; y<22; y++){
      for(int x=110; x< 128; x++){
	layerPanelImg->setPixel(x, 20+29*canvas->layernum + y, layer_bottom_backgroundImg->pixel(x,y));
      }
    }
  }



  QPixmap *layerPix = new QPixmap();
  layerPix->convertFromImage(*layerPanelImg, 0);

  QPainter p;
  p.begin( layerPix );
  p.setFont(QFont("lcfont", 8));

  if(canvas->currentlayer == 0){
    p.setPen( white );
  }
  else{
    p.setPen( black );
  }
  p.drawText(QRect(29,20+(canvas->layernum-1)*29+8,55 ,12),Qt::AlignCenter , canvas->layerName[0]);

  for(int i=1; i < canvas->layernum; i++){
    if(canvas->currentlayer == i){
      p.setPen( white );
    }
    else{
      p.setPen( black );
    }
    p.drawText(QRect(29,20+(canvas->layernum-1-i)*29+8, 55 ,12),Qt::AlignCenter , canvas->layerName[i]);
  }

  p.end();

  setBackgroundPixmap(*layerPix);
  delete layerPix;
}

Layer::~Layer()
{
  delete layerPanelImg;
  delete layer_normalImg;
  delete layer_normalImgOrg;
  delete layer_nomodeImg;
  delete layer_visibleImg;
  delete layer_hogoImg;
  delete layer_freeImg;
  delete layer_invisibleImg;
  delete layer_top_normalImg;
  delete layer_top_reverseImg;
  delete layer_bottom_normalImg;
  delete layer_bottom_reverseImg;
  delete layer_bottom_backgroundImg;

}


void Layer::changeOpacityArea(int x){

  int tmpAlpha;
  int tmpAlpha2;

  if(x<OPACITY_X)
    tmpAlpha = 0;
  else if(x>OPACITY_X+OPACITY_WIDTH)
    tmpAlpha = 100;	
  else
    tmpAlpha = x - (OPACITY_X);

  tmpAlpha2 = (int)((double)tmpAlpha*2.56);
  if(tmpAlpha2 > 256){
    tmpAlpha2 = 256;
  }

  layerPanelImg->setPixel(OPACITY_X+1+tmpAlpha,20+29*canvas->layernum+OPACITY_Y+1, 0xff000000);
  layerPanelImg->setPixel(OPACITY_X+1+tmpAlpha,20+29*canvas->layernum+OPACITY_Y+2, 0xffffffff);
  layerPanelImg->setPixel(OPACITY_X+1+tmpAlpha,20+29*canvas->layernum+OPACITY_Y+3, 0xffffffff);
  layerPanelImg->setPixel(OPACITY_X+1+tmpAlpha,20+29*canvas->layernum+OPACITY_Y+4, 0xff000000);

  layerPanelImg->setPixel(OPACITY_X  +tmpAlpha,20+29*canvas->layernum+OPACITY_Y+2, 0xff000000);
  layerPanelImg->setPixel(OPACITY_X  +tmpAlpha,20+29*canvas->layernum+OPACITY_Y+3, 0xff000000);
  layerPanelImg->setPixel(OPACITY_X  +tmpAlpha,20+29*canvas->layernum+OPACITY_Y+4, 0xff000000);

  layerPanelImg->setPixel(OPACITY_X+2+tmpAlpha,20+29*canvas->layernum+OPACITY_Y+2, 0xff000000);
  layerPanelImg->setPixel(OPACITY_X+2+tmpAlpha,20+29*canvas->layernum+OPACITY_Y+3, 0xff000000);
  layerPanelImg->setPixel(OPACITY_X+2+tmpAlpha,20+29*canvas->layernum+OPACITY_Y+4, 0xff000000);


  canvas->layerAlpha[canvas->currentlayer] =tmpAlpha;
  canvas->layerAlpha2[canvas->currentlayer] =tmpAlpha2;

}



void Layer::RebuildPanel(){
  if(canvas->huebar->y() == 0){
    canvas->layerPanelY = LAYERPANEL_Y2;
  }
  else{
    canvas->layerPanelY = 640 - 80 - (44+20+29*canvas->layernum);
  }
  if(!stickyFlag){
    move(LAYERPANEL_X, canvas->layerPanelY);
  }

  layerPanelImg = new QImage(148, 45+20+29*canvas->layernum, 32, 0, QImage::IgnoreEndian );


  for(int y=0; y<20; y++){
    for(int x=0; x<148; x++){
      layerPanelImg->setPixel(x,y,layer_top_normalImg->pixel(x,y));
    }
  }


  if(stickyFlag){
    for(int y=3; y<17; y++){
      for(int x=115; x<144; x++){
	layerPanelImg->setPixel(x,y,layer_top_reverseImg->pixel(x,y));
      }
    }
  }
  else{
    for(int y=3; y<17; y++){
      for(int x=115; x<144; x++){
	layerPanelImg->setPixel(x,y,layer_top_normalImg->pixel(x,y));
      }
    }
  }

  for(int i=0; i<canvas->layernum; i++){
    if(i != canvas->layernum-1){
      for(int y=0; y<21; y++){
	for(int x=0; x<62; x++){
	  layer_normalImg->setPixel(x+82,y+4,canvas->mode_normalImg->pixel(x+1,y+1+canvas->layerMode[canvas->layernum-i-1]*21));
	}
      }
    }
    else{
      for(int y=0; y<21; y++){
	for(int x=0; x<62; x++){
	  layer_normalImg->setPixel(x+82,y+4,layer_nomodeImg->pixel(x,y));
	}
      }
    }

    if(canvas->layerVisible[canvas->layernum-i-1]){
      for(int y=0; y<29; y++){
	for(int x=0; x<18; x++){
	  layer_normalImg->setPixel(x,y,layer_visibleImg->pixel(x,y));
	}
      }
    }
    else{
      for(int y=0; y<29; y++){
	for(int x=0; x<18; x++){
	  layer_normalImg->setPixel(x,y,layer_invisibleImg->pixel(x,y));
	}
      }
    }

    if(canvas->layernum-i-1 == 0){
      for(int y=0; y<29; y++){
	for(int x=19; x<34; x++){
	  layer_normalImg->setPixel(x,y,layer_normalImgOrg->pixel(x,y));
	}
      }
    }
    else if(canvas->layerDrawable[canvas->layernum-i-1]){
      for(int y=0; y<29; y++){
	for(int x=19; x<34; x++){
	  layer_normalImg->setPixel(x,y,layer_freeImg->pixel(x-19,y));
	}
      }
    }
    else{
      for(int y=0; y<29; y++){
	for(int x=19; x<34; x++){
	  layer_normalImg->setPixel(x,y,layer_hogoImg->pixel(x-19,y));
	}
      }
    }


    for(int y=0; y<29; y++){
      for(int x=0; x<148; x++){
	layerPanelImg->setPixel(x,y+20+i*29,layer_normalImg->pixel(x,y));
      }
    }
  }


  if(canvas->currentlayer == 0){
    for(int y=0; y<21; y++){
      for(int x=0; x<62; x++){
	layer_normalImg->setPixel(x+82,y+4,layer_nomodeImg->pixel(x,y));
      }
    }
  }
  else{
    for(int y=0; y<21; y++){
      for(int x=0; x<62; x++){
	layer_normalImg->setPixel(x+82,y+4,canvas->mode_normalImg->pixel(x+1,y+1+canvas->layerMode[canvas->currentlayer]*21));
      }
    }
  }

  if(canvas->layerVisible[canvas->currentlayer]){
    for(int y=0; y<29; y++){
      for(int x=0; x<18; x++){
	layer_normalImg->setPixel(x,y,layer_visibleImg->pixel(x,y));
      }
    }
  }
  else{
    for(int y=0; y<29; y++){
      for(int x=0; x<18; x++){
	layer_normalImg->setPixel(x,y,layer_invisibleImg->pixel(x,y));
      }
    }
  }


  if(canvas->currentlayer == 0){
    for(int y=0; y<29; y++){
      for(int x=19; x<34; x++){
	layer_normalImg->setPixel(x,y,layer_normalImgOrg->pixel(x,y));
      }
    }
  }
  else if(canvas->layerDrawable[canvas->currentlayer]){
    for(int y=0; y<29; y++){
      for(int x=19; x<34; x++){
	layer_normalImg->setPixel(x,y,layer_freeImg->pixel(x-19,y));
      }
    }
  }
  else{
    for(int y=0; y<29; y++){
      for(int x=19; x<34; x++){
	layer_normalImg->setPixel(x,y,layer_hogoImg->pixel(x-19,y));
      }
    }
  }
  

  for(int y=0; y<29; y++){
    for(int x=1; x<1+145; x++){
      if(x >= 82 && x <=143 && y>=4 && y <=24 && canvas->currentlayer != 0){
	layerPanelImg->setPixel(x,y+20+(canvas->layernum-canvas->currentlayer-1)*29,layer_normalImg->pixel(x,y));
      }
      else{
	layerPanelImg->setPixel(x,y+20+(canvas->layernum-canvas->currentlayer-1)*29,(~(layer_normalImg->pixel(x,y))) | 0xff000000);
      }
    }
  }


  for(int y=0; y<45; y++){
    for(int x=0; x<148; x++){
      layerPanelImg->setPixel(x,y+20+canvas->layernum*29,layer_bottom_normalImg->pixel(x,y));
    }
  }
  if(canvas->currentlayer == 0){
    if(canvas->layernum <= 1){
      for(int y=0; y<22; y++){
	for(int x=69; x< 148; x++){
	  layerPanelImg->setPixel(x, 20+29*canvas->layernum + y, layer_bottom_backgroundImg->pixel(x,y));
	}
      }
    }
    else{
      for(int y=0; y<22; y++){
	for(int x=90; x< 148; x++){
	  layerPanelImg->setPixel(x, 20+29*canvas->layernum + y, layer_bottom_backgroundImg->pixel(x,y));
	}
      }
    }
    for(int y=22; y<41; y++){
      for(int x=0; x< 148; x++){
	layerPanelImg->setPixel(x, 20+29*canvas->layernum + y, layer_bottom_backgroundImg->pixel(x,y));
      }
    }

  }
  if(canvas->currentlayer == 1){
    for(int y=0; y<22; y++){
      for(int x=128; x< 148; x++){
	layerPanelImg->setPixel(x, 20+29*canvas->layernum + y, layer_bottom_backgroundImg->pixel(x,y));
      }
    }
  }
  if(canvas->currentlayer == canvas->layernum -1){
    for(int y=0; y<22; y++){
      for(int x=110; x< 128; x++){
	layerPanelImg->setPixel(x, 20+29*canvas->layernum + y, layer_bottom_backgroundImg->pixel(x,y));
      }
    }
  }

  if(canvas->currentlayer != 0){
    changeOpacityArea(OPACITY_X + canvas->layerAlpha[canvas->currentlayer]);
  }

  QPixmap *layerPix = new QPixmap();
  layerPix->convertFromImage(*layerPanelImg, 0);

  QPainter p;
  p.begin( layerPix );
  p.setFont(QFont("lcfont", 8));

  if(canvas->currentlayer != 0){
    QString tmpStr = "100";
    tmpStr = tmpStr.sprintf("%d",canvas->layerAlpha[canvas->currentlayer]);
    p.setPen( black );
    p.drawText(QRect(OPACITY_X,20+29*canvas->layernum+OPACITY_Y-11, OPACITY_WIDTH-7, 12),Qt::AlignRight , tmpStr);
  }

  if(canvas->currentlayer == 0){
    p.setPen( white );
  }
  else{
    p.setPen( black );
  }
  p.drawText(QRect(29,20+(canvas->layernum-1)*29+8,55 ,12),Qt::AlignCenter , canvas->layerName[0]);

  for(int i=1; i < canvas->layernum; i++){
    if(canvas->currentlayer == i){
      p.setPen( white );
    }
    else{
      p.setPen( black );
    }
    p.drawText(QRect(29,20+(canvas->layernum-1-i)*29+8, 55 ,12),Qt::AlignCenter , canvas->layerName[i]);
  }

  p.end();

  resize(148, 45+20+29*canvas->layernum);
  setBackgroundPixmap(*layerPix);

  delete layerPix;

}

void Layer::showEvent( QShowEvent *e ){
  this->setActiveWindow();
  this->raise();

  for(int y=LAYERICON_Y; y<LAYERICON_Y+LAYERICON_HEIGHT; y++){
    for(int x=LAYERICON_X; x<LAYERICON_X+LAYERICON_WIDTH; x++){
      canvas->toolbar->toolbarImg.setPixel(x,y,canvas->toolbar->toolbar_reverseImg.pixel(x,y));
    }
  }
  QPixmap *toolbarPix = new QPixmap();
  toolbarPix->convertFromImage(canvas->toolbar->toolbarImg, 0);

  QPainter p2;
  p2.begin( toolbarPix );
  p2.setFont(QFont("lcfont", 8));
  p2.setPen( black );
  QString tmpStr = "0";
  tmpStr = tmpStr.sprintf("%d", canvas->currentlayer+1);
  p2.drawText(QRect(6,6, 9 ,10),Qt::AlignCenter , tmpStr);
  if(canvas->canvasType == VGA){  
    tmpStr = tmpStr.sprintf("%d", canvas->layernum);
  }
  else if(canvas->canvasType == QVGA){    
    tmpStr = tmpStr.sprintf("%dq", canvas->layernum);
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

  canvas->toolbar->setBackgroundPixmap(*toolbarPix);
  delete toolbarPix;

  if(!stickyFlag && (this->y() != canvas->layerPanelY)){
    move(LAYERPANEL_X, canvas->layerPanelY);
  }



  if(stickyFlag){
    for(int y=3; y<17; y++){
      for(int x=115; x<144; x++){
	layerPanelImg->setPixel(x,y,layer_top_reverseImg->pixel(x,y));
      }
    }
  }
  else{
    for(int y=3; y<17; y++){
      for(int x=115; x<144; x++){
	layerPanelImg->setPixel(x,y,layer_top_normalImg->pixel(x,y));
      }
    }
  }


  QPixmap *layerPix = new QPixmap();
  layerPix->convertFromImage(*layerPanelImg, 0);

  QPainter p;
  p.begin( layerPix );
  p.setFont(QFont("lcfont", 8));

  if(canvas->currentlayer != 0){
    QString tmpStr2 = "100";
    tmpStr2 = tmpStr2.sprintf("%d",canvas->layerAlpha[canvas->currentlayer]);
    p.setPen( black );
    p.drawText(QRect(OPACITY_X,20+29*canvas->layernum+OPACITY_Y-11, OPACITY_WIDTH-7, 12),Qt::AlignRight , tmpStr2);
  }

  if(canvas->currentlayer == 0){
    p.setPen( white );
  }
  else{
    p.setPen( black );
  }
  p.drawText(QRect(29,20+(canvas->layernum-1)*29+8, 55 ,12),Qt::AlignCenter , canvas->layerName[0]);

  for(int i=1; i < canvas->layernum; i++){
    if(canvas->currentlayer == i){
      p.setPen( white );
    }
    else{
      p.setPen( black );
    }
    p.drawText(QRect(29,20+(canvas->layernum-1-i)*29+8, 55 ,12),Qt::AlignCenter , canvas->layerName[i]);
  }

  p.end();

  setBackgroundPixmap(*layerPix);

  delete layerPix;

}

void Layer::hideEvent( QHideEvent *e ){
  for(int y=LAYERICON_Y; y<LAYERICON_Y+LAYERICON_HEIGHT; y++){
    for(int x=LAYERICON_X; x<LAYERICON_X+LAYERICON_WIDTH; x++){
      canvas->toolbar->toolbarImg.setPixel(x,y,canvas->toolbar->toolbar_normalImg.pixel(x,y));
    }
  }
  QPixmap *toolbarPix = new QPixmap();
  toolbarPix->convertFromImage(canvas->toolbar->toolbarImg, 0);

  QPainter p2;
  p2.begin( toolbarPix );
  p2.setFont(QFont("lcfont", 8));
  p2.setPen( black );
  QString tmpStr = "0";
  tmpStr = tmpStr.sprintf("%d", canvas->currentlayer+1);
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

  canvas->toolbar->setBackgroundPixmap(*toolbarPix);
  delete toolbarPix;
}



void Layer::mousePressEvent( QMouseEvent *e )
{
  int toolsize=0;
  closePressFlag = false;
  minimizePressFlag = false;
  opacitySelectingFlag = false;
  opacityMinusSelectFlag = false;
  opacityPlusSelectFlag = false;

  if(!this->isTopLevel()){
    this->setActiveWindow();
    this->raise();
  }

  if(e->y() >= 0 && e->y() < 17){
    if(e->x() >= 0 && e->x() < 115){
      prev_x = e->x();
      prev_y = e->y();

      moveFlag = true;
      stickyFlag = true;

      for(int y=3; y<17; y++){
	for(int x=115; x<144; x++){
	  layerPanelImg->setPixel(x,y,layer_top_reverseImg->pixel(x,y));
	}
      }
      QPixmap *layerPix = new QPixmap();
      layerPix->convertFromImage(*layerPanelImg, 0);

      QPainter p;
      p.begin( layerPix );
      p.setFont(QFont("lcfont", 8));

      if(canvas->currentlayer != 0){
	QString tmpStr = "100";
	tmpStr = tmpStr.sprintf("%d",canvas->layerAlpha[canvas->currentlayer]);
	p.setPen( black );
	p.drawText(QRect(OPACITY_X,20+29*canvas->layernum+OPACITY_Y-11, OPACITY_WIDTH-7, 12),Qt::AlignRight , tmpStr);
      }

      if(canvas->currentlayer == 0){
	p.setPen( white );
      }
      else{
	p.setPen( black );
      }
      p.drawText(QRect(29,20+(canvas->layernum-1)*29+8, 55 ,12),Qt::AlignCenter , canvas->layerName[0]);

      for(int i=1; i < canvas->layernum; i++){
	if(canvas->currentlayer == i){
	  p.setPen( white );
	}
	else{
	  p.setPen( black );
	}
	p.drawText(QRect(29,20+(canvas->layernum-1-i)*29+8, 55 ,12),Qt::AlignCenter , canvas->layerName[i]);
      }

      p.end();

      setBackgroundPixmap(*layerPix);
      delete layerPix;

      return;
    }
    else if(e->x() >= 115 && e->x() < 129){
      minimizePressFlag = true;
    }
    else if(e->x() >= 129 && e->x() < 144){
      closePressFlag = true;
    }
  }

  if(minimizePressFlag){
    for(int y=3; y<17; y++){
      for(int x=115; x<129; x++){
	layerPanelImg->setPixel(x,y,(~(layer_top_reverseImg->pixel(x,y))) | 0xff000000);
      }
    }
  }
  else if(closePressFlag){
    for(int y=3; y<17; y++){
      for(int x=129; x<144; x++){
	layerPanelImg->setPixel(x,y,(~(layer_top_reverseImg->pixel(x,y))) | 0xff000000);
      }
    }
  }
  else if(stickyFlag){
    for(int y=3; y<17; y++){
      for(int x=115; x<144; x++){
	layerPanelImg->setPixel(x,y,layer_top_reverseImg->pixel(x,y));
      }
    }
  }
  else{
    for(int y=3; y<17; y++){
      for(int x=115; x<144; x++){
	layerPanelImg->setPixel(x,y,layer_top_normalImg->pixel(x,y));
      }
    }
  }

  if(e->y() >= 0 && e->y() < 17){
    if(e->x() >= 115 && e->x() < 129){
      for(int y=3; y<17; y++){
	for(int x=115; x<129; x++){
	  layerPanelImg->setPixel(x,y,(~(layer_top_reverseImg->pixel(x,y))) | 0xff000000);
	}
      }
    }
    else if(e->x() >= 130 && e->x() < 144){
      for(int y=3; y<17; y++){
	for(int x=130; x<144; x++){
	  layerPanelImg->setPixel(x,y,(~(layer_top_reverseImg->pixel(x,y))) | 0xff000000);
	}
      }
    }
  }


  for(int i=0; i<canvas->layernum; i++){
    if(i != canvas->layernum-1){
      for(int y=0; y<21; y++){
	for(int x=0; x<62; x++){
	  layer_normalImg->setPixel(x+82,y+4,canvas->mode_normalImg->pixel(x+1,y+1+canvas->layerMode[canvas->layernum-i-1]*21));
	}
      }
    }
    else{
      for(int y=0; y<21; y++){
	for(int x=0; x<62; x++){
	  layer_normalImg->setPixel(x+82,y+4,layer_nomodeImg->pixel(x,y));
	}
      }
    }

    if(canvas->layerVisible[canvas->layernum-i-1]){
      for(int y=0; y<29; y++){
	for(int x=0; x<18; x++){
	  layer_normalImg->setPixel(x,y,layer_visibleImg->pixel(x,y));
	}
      }
    }
    else{
      for(int y=0; y<29; y++){
	for(int x=0; x<18; x++){
	  layer_normalImg->setPixel(x,y,layer_invisibleImg->pixel(x,y));
	}
      }
    }

    if(canvas->layernum-i-1 == 0){
      for(int y=0; y<29; y++){
	for(int x=19; x<34; x++){
	  layer_normalImg->setPixel(x,y,layer_normalImgOrg->pixel(x,y));
	}
      }
    }
    else if(canvas->layerDrawable[canvas->layernum-i-1]){
      for(int y=0; y<29; y++){
	for(int x=19; x<34; x++){
	  layer_normalImg->setPixel(x,y,layer_freeImg->pixel(x-19,y));
	}
      }
    }
    else{
      for(int y=0; y<29; y++){
	for(int x=19; x<34; x++){
	  layer_normalImg->setPixel(x,y,layer_hogoImg->pixel(x-19,y));
	}
      }
    }


    for(int y=0; y<29; y++){
      for(int x=0; x<148; x++){
	layerPanelImg->setPixel(x,y+20+i*29,layer_normalImg->pixel(x,y));
      }
    }

  }



  int tmpIndex = -1;
  if(e->y() >= 20  && e->y() < 20+29*canvas->layernum){
    tmpIndex = canvas->layernum - ((e->y()-20)/29) -1;
  }
  if(tmpIndex != -1){

    for(int y=0; y<21; y++){
      for(int x=0; x<62; x++){
	if(e->x() >= 82 && e->x() <= 143){
	  if(tmpIndex == 0){
	    layer_normalImg->setPixel(x+82,y+4,layer_nomodeImg->pixel(x,y));
	  }
	  else{
	    layer_normalImg->setPixel(x+82,y+4,canvas->mode_reverseImg->pixel(x+1,y+1+canvas->layerMode[tmpIndex]*21));
	  }
	}
	else{
	  if(tmpIndex == 0){
	    layer_normalImg->setPixel(x+82,y+4,layer_nomodeImg->pixel(x,y));
	  }
	  else{
	    layer_normalImg->setPixel(x+82,y+4,canvas->mode_normalImg->pixel(x+1,y+1+canvas->layerMode[tmpIndex]*21));      
	  }
	}
      }


      if(canvas->layerVisible[tmpIndex]){
	for(int y=0; y<29; y++){
	  for(int x=0; x<18; x++){
	    layer_normalImg->setPixel(x,y,layer_visibleImg->pixel(x,y));
	  }
	}
      }
      else{
	for(int y=0; y<29; y++){
	  for(int x=0; x<18; x++){
	    layer_normalImg->setPixel(x,y,layer_invisibleImg->pixel(x,y));
	  }
	}
      }


      if(tmpIndex == 0){
	for(int y=0; y<29; y++){
	  for(int x=19; x<34; x++){
	    layer_normalImg->setPixel(x,y,layer_normalImgOrg->pixel(x,y));
	  }
	}
      }
      else if(canvas->layerDrawable[tmpIndex]){
	for(int y=0; y<29; y++){
	  for(int x=19; x<34; x++){
	    layer_normalImg->setPixel(x,y,layer_freeImg->pixel(x-19,y));
	  }
	}
      }
      else{
	for(int y=0; y<29; y++){
	  for(int x=19; x<34; x++){
	    layer_normalImg->setPixel(x,y,layer_hogoImg->pixel(x-19,y));
	  }
	}
      }



      for(int y=0; y<29; y++){
	for(int x=1; x<1+145; x++){
	  if(x >= 82 && x<=143 && y>=4 && y <=24 && tmpIndex != 0){
	    layerPanelImg->setPixel(x,y+20+(canvas->layernum-tmpIndex-1)*29,layer_normalImg->pixel(x,y));
	  }
	  else{
	    layerPanelImg->setPixel(x,y+20+(canvas->layernum-tmpIndex-1)*29,(~(layer_normalImg->pixel(x,y))) | 0xff000000);
	  }
	}
      }

    }

  }
  else{
    for(int y=0; y<21; y++){
      for(int x=0; x<62; x++){
	if(canvas->currentlayer == 0){
	  layer_normalImg->setPixel(x+82,y+4,layer_nomodeImg->pixel(x,y));
	}
	else{
	  layer_normalImg->setPixel(x+82,y+4,canvas->mode_normalImg->pixel(x+1,y+1+canvas->layerMode[canvas->currentlayer]*21));      
	}
      }
    }

    if(canvas->layerVisible[canvas->currentlayer]){
      for(int y=0; y<29; y++){
	for(int x=0; x<18; x++){
	  layer_normalImg->setPixel(x,y,layer_visibleImg->pixel(x,y));
	}
      }
    }
    else{
      for(int y=0; y<29; y++){
	for(int x=0; x<18; x++){
	  layer_normalImg->setPixel(x,y,layer_invisibleImg->pixel(x,y));
	}
      }
    }

    
    if(canvas->currentlayer == 0){
      for(int y=0; y<29; y++){
	for(int x=19; x<34; x++){
	  layer_normalImg->setPixel(x,y,layer_normalImgOrg->pixel(x,y));
	}
      }
    }
    else if(canvas->layerDrawable[canvas->currentlayer]){
      for(int y=0; y<29; y++){
	for(int x=19; x<34; x++){
	  layer_normalImg->setPixel(x,y,layer_freeImg->pixel(x-19,y));
	}
      }
    }
    else{
      for(int y=0; y<29; y++){
	for(int x=19; x<34; x++){
	  layer_normalImg->setPixel(x,y,layer_hogoImg->pixel(x-19,y));
	}
      }
    }



    for(int y=0; y<29; y++){
      for(int x=1; x<1+145; x++){
	if(x >= 82 && x <=143 && y>=4 && y <=24 && canvas->currentlayer != 0){
	  layerPanelImg->setPixel(x,y+20+(canvas->layernum-canvas->currentlayer-1)*29,layer_normalImg->pixel(x,y));
	}
	else{
	  layerPanelImg->setPixel(x,y+20+(canvas->layernum-canvas->currentlayer-1)*29,(~(layer_normalImg->pixel(x,y))) | 0xff000000);
	}
      }
    }
  }



  int left = 0;

  if(e->y() >= 20+29*canvas->layernum + 2  && e->y() < 20+29*canvas->layernum + 21){
    if(e->x() >= 3 && e->x() <= 21){
      left = 3;
    }
    else if(e->x() >= 25 && e->x() <= 43){
      left = 25;
    }
    else if(e->x() >= 47 && e->x() <= 65){
      left = 47;
    }
    else if(e->x() >= 69 && e->x() <= 87){
      left = 69;
    }
    else if(e->x() >= 90 && e->x() <= 108){
      left = 90;
    }
    else if(e->x() >= 112 && e->x() <= 126){
      left = 110;
    }
    else if(e->x() >= 130 && e->x() <= 144){
      left = 129;
    }
 
    if(left != 0){
      for(int y=0; y<41; y++){
	for(int x=0; x< 148; x++){
	  layerPanelImg->setPixel(x, 20+29*canvas->layernum + y,layer_bottom_normalImg->pixel(x,y));
	}
      }
      for(int y=0; y<22; y++){
	for(int x=left; x<left+19; x++){
	  layerPanelImg->setPixel(x, 20+29*canvas->layernum + y,layer_bottom_reverseImg->pixel(x,y));
	}
      }
      if(canvas->currentlayer == 0){
	if(canvas->layernum <= 1){
	  for(int y=0; y<22; y++){
	    for(int x=69; x< 148; x++){
	      layerPanelImg->setPixel(x, 20+29*canvas->layernum + y, layer_bottom_backgroundImg->pixel(x,y));

	    }
	  }
	}
	else{
	  for(int y=0; y<22; y++){
	    for(int x=90; x< 148; x++){
	      layerPanelImg->setPixel(x, 20+29*canvas->layernum + y, layer_bottom_backgroundImg->pixel(x,y));
	    }
	  }
	}
	for(int y=22; y<41; y++){
	  for(int x=0; x< 148; x++){
	    layerPanelImg->setPixel(x, 20+29*canvas->layernum + y, layer_bottom_backgroundImg->pixel(x,y));
	  }
	}

      }
      if(canvas->currentlayer == 1){
	for(int y=0; y<22; y++){
	  for(int x=128; x< 148; x++){
	    layerPanelImg->setPixel(x, 20+29*canvas->layernum + y, layer_bottom_backgroundImg->pixel(x,y));
	  }
	}
      }
      if(canvas->currentlayer == canvas->layernum -1){
	for(int y=0; y<22; y++){
	  for(int x=110; x< 128; x++){
	    layerPanelImg->setPixel(x, 20+29*canvas->layernum + y, layer_bottom_backgroundImg->pixel(x,y));
	  }
	}
      }


    }

  }
  else if(e->y() >= 20+29*canvas->layernum + 21  && canvas->currentlayer != 0){
    if(e->x() <= 17){
      opacityMinusSelectFlag = true;
      left = 3;
    }
    else if(e->x() >= 129){
      opacityPlusSelectFlag = true;
      left = 129;
    }


    if(left != 0){ 
      for(int y=0; y<41; y++){
	for(int x=0; x< 148; x++){
	  layerPanelImg->setPixel(x, 20+29*canvas->layernum + y,layer_bottom_normalImg->pixel(x,y));
	}
      }
      for(int y=25; y<25+16; y++){
	for(int x=left; x<left+16; x++){
	  layerPanelImg->setPixel(x, 20+29*canvas->layernum + y,layer_bottom_reverseImg->pixel(x,y));
	}
      }
      if(canvas->currentlayer == 0){
	if(canvas->layernum <= 1){
	  for(int y=0; y<22; y++){
	    for(int x=69; x< 148; x++){
	      layerPanelImg->setPixel(x, 20+29*canvas->layernum + y, layer_bottom_backgroundImg->pixel(x,y));
	    }
	  }
	}
	else{
	  for(int y=0; y<22; y++){
	    for(int x=90; x< 148; x++){
	      layerPanelImg->setPixel(x, 20+29*canvas->layernum + y, layer_bottom_backgroundImg->pixel(x,y));
	    }
	  }
	}
	for(int y=22; y<41; y++){
	  for(int x=0; x< 148; x++){
	    layerPanelImg->setPixel(x, 20+29*canvas->layernum + y, layer_bottom_backgroundImg->pixel(x,y));
	  }
	}

      }
      if(canvas->currentlayer == 1){
	for(int y=0; y<22; y++){
	  for(int x=128; x< 148; x++){
	    layerPanelImg->setPixel(x, 20+29*canvas->layernum + y, layer_bottom_backgroundImg->pixel(x,y));
	  }
	}
      }
      if(canvas->currentlayer == canvas->layernum -1){
	for(int y=0; y<22; y++){
	  for(int x=110; x< 128; x++){
	    layerPanelImg->setPixel(x, 20+29*canvas->layernum + y, layer_bottom_backgroundImg->pixel(x,y));
	  }
	}
      }


      changeOpacityArea(OPACITY_X + canvas->layerAlpha[canvas->currentlayer]);

    }
    else{
      opacitySelectingFlag = true;

      layerPanelImg->setPixel(OPACITY_X+1+canvas->layerAlpha[canvas->currentlayer],20+29*canvas->layernum+OPACITY_Y+1, 0xff9B9B9B);
      layerPanelImg->setPixel(OPACITY_X+1+canvas->layerAlpha[canvas->currentlayer],20+29*canvas->layernum+OPACITY_Y+2, 0xff9B9B9B);
      layerPanelImg->setPixel(OPACITY_X+1+canvas->layerAlpha[canvas->currentlayer],20+29*canvas->layernum+OPACITY_Y+3, 0xff9B9B9B);
      layerPanelImg->setPixel(OPACITY_X+1+canvas->layerAlpha[canvas->currentlayer],20+29*canvas->layernum+OPACITY_Y+4, 0xff9B9B9B);

      layerPanelImg->setPixel(OPACITY_X  +canvas->layerAlpha[canvas->currentlayer],20+29*canvas->layernum+OPACITY_Y+2, 0xff9B9B9B);
      layerPanelImg->setPixel(OPACITY_X  +canvas->layerAlpha[canvas->currentlayer],20+29*canvas->layernum+OPACITY_Y+3, 0xff9B9B9B);
      layerPanelImg->setPixel(OPACITY_X  +canvas->layerAlpha[canvas->currentlayer],20+29*canvas->layernum+OPACITY_Y+4, 0xff9B9B9B);

      layerPanelImg->setPixel(OPACITY_X+2+canvas->layerAlpha[canvas->currentlayer],20+29*canvas->layernum+OPACITY_Y+2, 0xff9B9B9B);
      layerPanelImg->setPixel(OPACITY_X+2+canvas->layerAlpha[canvas->currentlayer],20+29*canvas->layernum+OPACITY_Y+3, 0xff9B9B9B);
      layerPanelImg->setPixel(OPACITY_X+2+canvas->layerAlpha[canvas->currentlayer],20+29*canvas->layernum+OPACITY_Y+4, 0xff9B9B9B);

      changeOpacityArea(e->x());


      for(int x=OPACITY_X+75; x<OPACITY_X+OPACITY_WIDTH-7;x++)
	for(int y=20+29*canvas->layernum+OPACITY_Y-11; y<20+29*canvas->layernum+OPACITY_Y;y++)
	  layerPanelImg->setPixel(x,y,0xff9B9B9B);
    }
  }

  QPixmap *layerPix = new QPixmap();
  layerPix->convertFromImage(*layerPanelImg, 0);

  QPainter p;
  p.begin( layerPix );
  p.setFont(QFont("lcfont", 8));

  if(canvas->currentlayer != 0){
    QString tmpStr = "100";
    tmpStr = tmpStr.sprintf("%d",canvas->layerAlpha[canvas->currentlayer]);
    p.setPen( black );
    p.drawText(QRect(OPACITY_X,20+29*canvas->layernum+OPACITY_Y-11, OPACITY_WIDTH-7, 12),Qt::AlignRight , tmpStr);
  }

  if(canvas->currentlayer == 0){
    p.setPen( white );
  }
  else{
    p.setPen( black );
  }
  p.drawText(QRect(29,20+(canvas->layernum-1)*29+8, 55 ,12),Qt::AlignCenter , canvas->layerName[0]);

  for(int i=1; i < canvas->layernum; i++){
    if(canvas->currentlayer == i){
      p.setPen( white );
    }
    else{
      p.setPen( black );
    }
    p.drawText(QRect(29,20+(canvas->layernum-1-i)*29+8, 55 ,12),Qt::AlignCenter , canvas->layerName[i]);
  }

  p.end();

  setBackgroundPixmap(*layerPix);

  delete layerPix;

}


void Layer::mouseMoveEvent( QMouseEvent *e )
{
  int next_x, next_y;

  if(moveFlag){
    next_x = x()+e->x()-prev_x;
    next_y = y()+e->y()-prev_y;
  
    if(next_x < -100){
      next_x = -100;
    }
    else if(next_x > 480-18){
      next_x = 480-18;
    }

    if(next_y < 0){
      next_y = 0;
    }
    else if(next_y > 640-18){
      next_y = 640-18;
    }

    int xdelta = next_x - this->x();    
    int ydelta =  next_y - this->y();

    move(next_x, next_y);
    canvas->layerModePanel->move(canvas->layerModePanel->x()+xdelta, canvas->layerModePanel->y()+ydelta);
    return;
  }


  if(stickyFlag){
    for(int y=3; y<17; y++){
      for(int x=115; x<144; x++){
	layerPanelImg->setPixel(x,y,layer_top_reverseImg->pixel(x,y));
      }
    }
  }
  else{
    for(int y=3; y<17; y++){
      for(int x=115; x<144; x++){
	layerPanelImg->setPixel(x,y,layer_top_normalImg->pixel(x,y));
      }
    }
  }

  if(e->y() >= 0 && e->y() < 17){
    if(e->x() >= 115 && e->x() < 129){
      minimizePressFlag = true;
      for(int y=3; y<17; y++){
	for(int x=115; x<129; x++){
	  layerPanelImg->setPixel(x,y,(~(layer_top_reverseImg->pixel(x,y))) | 0xff000000);
	}
      }
    }
    else if(e->x() >= 129){
      closePressFlag = true;
      for(int y=3; y<17; y++){
	for(int x=130; x<144; x++){
	  layerPanelImg->setPixel(x,y,(~(layer_top_reverseImg->pixel(x,y))) | 0xff000000);
	}
      }
    }
    else{
      closePressFlag = false;
      minimizePressFlag = false;
    }


    QPixmap *layerPix = new QPixmap();
    layerPix->convertFromImage(*layerPanelImg, 0);

    QPainter p;
    p.begin( layerPix );
    p.setFont(QFont("lcfont", 8));

    if(canvas->currentlayer != 0){
      QString tmpStr = "100";
      tmpStr = tmpStr.sprintf("%d",canvas->layerAlpha[canvas->currentlayer]);
      p.setPen( black );
      p.drawText(QRect(OPACITY_X,20+29*canvas->layernum+OPACITY_Y-11, OPACITY_WIDTH-7, 12),Qt::AlignRight , tmpStr);
    }

    if(canvas->currentlayer == 0){
      p.setPen( white );
    }
    else{
      p.setPen( black );
    }
    p.drawText(QRect(29,20+(canvas->layernum-1)*29+8, 55 ,12),Qt::AlignCenter , canvas->layerName[0]);

    for(int i=1; i < canvas->layernum; i++){
      if(canvas->currentlayer == i){
	p.setPen( white );
      }
      else{
	p.setPen( black );
      }
      p.drawText(QRect(29,20+(canvas->layernum-1-i)*29+8, 55 ,12),Qt::AlignCenter , canvas->layerName[i]);
    }

    p.end();

    setBackgroundPixmap(*layerPix);

    delete layerPix;

    return;

  }

  if(e->y() >= 20  && e->y() < 20+29*canvas->layernum){
    for(int i=0; i<canvas->layernum; i++){
      if(i != canvas->layernum-1){
	for(int y=0; y<21; y++){
	  for(int x=0; x<62; x++){
	    layer_normalImg->setPixel(x+82,y+4,canvas->mode_normalImg->pixel(x+1,y+1+canvas->layerMode[canvas->layernum-i-1]*21));
	  }
	}
      }
      else{
	for(int y=0; y<21; y++){
	  for(int x=0; x<62; x++){
	    layer_normalImg->setPixel(x+82,y+4,layer_nomodeImg->pixel(x,y));
	  }
	}
      }

      if(canvas->layerVisible[canvas->layernum-i-1]){
	for(int y=0; y<29; y++){
	  for(int x=0; x<18; x++){
	    layer_normalImg->setPixel(x,y,layer_visibleImg->pixel(x,y));
	  }
	}
      }
      else{
	for(int y=0; y<29; y++){
	  for(int x=0; x<18; x++){
	    layer_normalImg->setPixel(x,y,layer_invisibleImg->pixel(x,y));
	  }
	}
      }

      if(canvas->layernum-i-1 == 0){
	for(int y=0; y<29; y++){
	  for(int x=19; x<34; x++){
	    layer_normalImg->setPixel(x,y,layer_normalImgOrg->pixel(x,y));
	  }
	}
      }
      else if(canvas->layerDrawable[canvas->layernum-i-1]){
	for(int y=0; y<29; y++){
	  for(int x=19; x<34; x++){
	    layer_normalImg->setPixel(x,y,layer_freeImg->pixel(x-19,y));
	  }
	}
      }
      else{
	for(int y=0; y<29; y++){
	  for(int x=19; x<34; x++){
	    layer_normalImg->setPixel(x,y,layer_hogoImg->pixel(x-19,y));
	  }
	}
      }
  

      for(int y=0; y<29; y++){
	for(int x=0; x<148; x++){
	  layerPanelImg->setPixel(x,y+20+i*29,layer_normalImg->pixel(x,y));
	}
      }
    }


    int tmpIndex = -1;
    if(e->y() >= 20  && e->y() < 20+29*canvas->layernum){
      tmpIndex = canvas->layernum - ((e->y()-20)/29) -1;
    }
    if(tmpIndex != -1){

      for(int y=0; y<21; y++){
	for(int x=0; x<62; x++){
	  if(e->x() >= 82 && e->x() <= 143){
	    if(tmpIndex == 0){
	      layer_normalImg->setPixel(x+82,y+4,layer_nomodeImg->pixel(x,y));
	    }
	    else{
	      layer_normalImg->setPixel(x+82,y+4,canvas->mode_reverseImg->pixel(x+1,y+1+canvas->layerMode[tmpIndex]*21));
	    }
	  }
	  else{
	    if(tmpIndex == 0){
	      layer_normalImg->setPixel(x+82,y+4,layer_nomodeImg->pixel(x,y));
	    }
	    else{
	      layer_normalImg->setPixel(x+82,y+4,canvas->mode_normalImg->pixel(x+1,y+1+canvas->layerMode[tmpIndex]*21));      
	    }
	  }

	}

	if(canvas->layerVisible[tmpIndex]){
	  for(int y=0; y<29; y++){
	    for(int x=0; x<18; x++){
	      layer_normalImg->setPixel(x,y,layer_visibleImg->pixel(x,y));
	    }
	  }
	}
	else{
	  for(int y=0; y<29; y++){
	    for(int x=0; x<18; x++){
	      layer_normalImg->setPixel(x,y,layer_invisibleImg->pixel(x,y));
	    }
	  }
	}

	if(tmpIndex == 0){
	  for(int y=0; y<29; y++){
	    for(int x=19; x<34; x++){
	      layer_normalImg->setPixel(x,y,layer_normalImgOrg->pixel(x,y));
	    }
	  }
	}
	else if(canvas->layerDrawable[tmpIndex]){
	  for(int y=0; y<29; y++){
	    for(int x=19; x<34; x++){
	      layer_normalImg->setPixel(x,y,layer_freeImg->pixel(x-19,y));
	    }
	  }
	}
	else{
	  for(int y=0; y<29; y++){
	    for(int x=19; x<34; x++){
	      layer_normalImg->setPixel(x,y,layer_hogoImg->pixel(x-19,y));
	    }
	  }
	}


	for(int y=0; y<29; y++){
	  for(int x=1; x<1+145; x++){
	    if(x >= 82 && x <=143 && y>=4 && y <=24 && tmpIndex != 0){
	      layerPanelImg->setPixel(x,y+20+(canvas->layernum-tmpIndex-1)*29,layer_normalImg->pixel(x,y));
	    }
	    else{
	      layerPanelImg->setPixel(x,y+20+(canvas->layernum-tmpIndex-1)*29,(~(layer_normalImg->pixel(x,y))) | 0xff000000);
	    }
	  }
	}

      }

    }
    else{
      for(int y=0; y<21; y++){
	for(int x=0; x<62; x++){
	  if(canvas->currentlayer == 0){
	    layer_normalImg->setPixel(x+82,y+4,layer_nomodeImg->pixel(x,y));
	  }
	  else{
	    layer_normalImg->setPixel(x+82,y+4,canvas->mode_normalImg->pixel(x+1,y+1+canvas->layerMode[canvas->currentlayer]*21));      
	  }
	}
      }

      if(canvas->layerVisible[canvas->currentlayer]){
	for(int y=0; y<29; y++){
	  for(int x=0; x<18; x++){
	    layer_normalImg->setPixel(x,y,layer_visibleImg->pixel(x,y));
	  }
	}
      }
      else{
	for(int y=0; y<29; y++){
	  for(int x=0; x<18; x++){
	    layer_normalImg->setPixel(x,y,layer_invisibleImg->pixel(x,y));
	  }
	}
      }


      if(canvas->currentlayer == 0){
	for(int y=0; y<29; y++){
	  for(int x=19; x<34; x++){
	    layer_normalImg->setPixel(x,y,layer_normalImgOrg->pixel(x,y));
	  }
	}
      }
      else if(canvas->layerDrawable[canvas->currentlayer]){
	for(int y=0; y<29; y++){
	  for(int x=19; x<34; x++){
	    layer_normalImg->setPixel(x,y,layer_freeImg->pixel(x-19,y));
	  }
	}
      }
      else{
	for(int y=0; y<29; y++){
	  for(int x=19; x<34; x++){
	    layer_normalImg->setPixel(x,y,layer_hogoImg->pixel(x-19,y));
	  }
	}
      }


      for(int y=0; y<29; y++){
	for(int x=1; x<1+145; x++){
	  if(x >= 82 && x <=143 && y>=4 && y <=24 && canvas->currentlayer != 0){
	    layerPanelImg->setPixel(x,y+20+(canvas->layernum-canvas->currentlayer-1)*29,layer_normalImg->pixel(x,y));
	  }
	  else{
	    layerPanelImg->setPixel(x,y+20+(canvas->layernum-canvas->currentlayer-1)*29,(~(layer_normalImg->pixel(x,y))) | 0xff000000);
	  }
	}
      }
    }

    QPixmap *layerPix = new QPixmap();
    layerPix->convertFromImage(*layerPanelImg, 0);

    QPainter p;
    p.begin( layerPix );
    p.setFont(QFont("lcfont", 8));

    if(canvas->currentlayer != 0){
      QString tmpStr = "100";
      tmpStr = tmpStr.sprintf("%d",canvas->layerAlpha[canvas->currentlayer]);
      p.setPen( black );
      p.drawText(QRect(OPACITY_X,20+29*canvas->layernum+OPACITY_Y-11, OPACITY_WIDTH-7, 12),Qt::AlignRight , tmpStr);
    }

    if(canvas->currentlayer == 0){
      p.setPen( white );
    }
    else{
      p.setPen( black );
    }
    p.drawText(QRect(29,20+(canvas->layernum-1)*29+8, 55 ,12),Qt::AlignCenter , canvas->layerName[0]);

    for(int i=1; i < canvas->layernum; i++){
      if(canvas->currentlayer == i){
	p.setPen( white );
      }
      else{
	p.setPen( black );
      }
      p.drawText(QRect(29,20+(canvas->layernum-1-i)*29+8, 55 ,12),Qt::AlignCenter , canvas->layerName[i]);
    }

    p.end();

    setBackgroundPixmap(*layerPix);

    delete layerPix;

    return;

  }

  int left = 0;

  if(e->y() >= 20+29*canvas->layernum + 2  && e->y() < 20+29*canvas->layernum + 21){
    if(e->x() >= 3 && e->x() <= 21){
      left = 3;
    }
    else if(e->x() >= 25 && e->x() <= 43){
      left = 25;
    }
    else if(e->x() >= 47 && e->x() <= 65){
      left = 47;
    }
    else if(e->x() >= 69 && e->x() <= 87){
      left = 69;
    }
    else if(e->x() >= 90 && e->x() <= 108){
      left = 90;
    }
    else if(e->x() >= 112 && e->x() <= 126){
      left = 110;
    }
    else if(e->x() >= 130 && e->x() <= 144){
      left = 129;
    }

    if(left != 0){ 
      for(int y=0; y<41; y++){
	for(int x=0; x< 148; x++){
	  layerPanelImg->setPixel(x, 20+29*canvas->layernum + y,layer_bottom_normalImg->pixel(x,y));
	}
      }
      for(int y=0; y<22; y++){
	for(int x=left; x<left+19; x++){
	  layerPanelImg->setPixel(x, 20+29*canvas->layernum + y,layer_bottom_reverseImg->pixel(x,y));
	}
      }
      if(canvas->currentlayer == 0){
	if(canvas->layernum <= 1){
	  for(int y=0; y<22; y++){
	    for(int x=69; x< 148; x++){
	      layerPanelImg->setPixel(x, 20+29*canvas->layernum + y, layer_bottom_backgroundImg->pixel(x,y));
	    }
	  }
	}
	else{
	  for(int y=0; y<22; y++){
	    for(int x=90; x< 148; x++){
	      layerPanelImg->setPixel(x, 20+29*canvas->layernum + y, layer_bottom_backgroundImg->pixel(x,y));
	    }
	  }
	}
	for(int y=22; y<41; y++){
	  for(int x=0; x< 148; x++){
	    layerPanelImg->setPixel(x, 20+29*canvas->layernum + y, layer_bottom_backgroundImg->pixel(x,y));
	  }
	}

      }
      if(canvas->currentlayer == 1){
	for(int y=0; y<22; y++){
	  for(int x=128; x< 148; x++){
	    layerPanelImg->setPixel(x, 20+29*canvas->layernum + y, layer_bottom_backgroundImg->pixel(x,y));
	  }
	}
      }
      if(canvas->currentlayer == canvas->layernum -1){
	for(int y=0; y<22; y++){
	  for(int x=110; x< 128; x++){
	    layerPanelImg->setPixel(x, 20+29*canvas->layernum + y, layer_bottom_backgroundImg->pixel(x,y));
	  }
	}
      }
    }
    QPixmap *layerPix = new QPixmap();
    layerPix->convertFromImage(*layerPanelImg, 0);

    QPainter p;
    p.begin( layerPix );
    p.setFont(QFont("lcfont", 8));

    if(canvas->currentlayer != 0){
      QString tmpStr = "100";
      tmpStr = tmpStr.sprintf("%d",canvas->layerAlpha[canvas->currentlayer]);
      p.setPen( black );
      p.drawText(QRect(OPACITY_X,20+29*canvas->layernum+OPACITY_Y-11, OPACITY_WIDTH-7, 12),Qt::AlignRight , tmpStr);
    }

    if(canvas->currentlayer == 0){
      p.setPen( white );
    }
    else{
      p.setPen( black );
    }
    p.drawText(QRect(29,20+(canvas->layernum-1)*29+8, 55 ,12),Qt::AlignCenter , canvas->layerName[0]);

    for(int i=1; i < canvas->layernum; i++){
      if(canvas->currentlayer == i){
	p.setPen( white );
      }
      else{
	p.setPen( black );
      }
      p.drawText(QRect(29,20+(canvas->layernum-1-i)*29+8, 55 ,12),Qt::AlignCenter , canvas->layerName[i]);
    }

    p.end();

    setBackgroundPixmap(*layerPix);
    delete layerPix;

    return;
  }
  else if(e->y() >= 20+29*canvas->layernum + 21 && canvas->currentlayer != 0){
    //OpacityArea

    if(e->x() <= 17 && !opacitySelectingFlag){
      left = 3;
    }
    else if(e->x() >= 129 && !opacitySelectingFlag){
      left = 129;
    }

    if(left != 0){ 
      for(int y=0; y<41; y++){
	for(int x=0; x< 148; x++){
	  layerPanelImg->setPixel(x, 20+29*canvas->layernum + y,layer_bottom_normalImg->pixel(x,y));
	}
      }
      for(int y=25; y<25+16; y++){
	for(int x=left; x<left+16; x++){
	  layerPanelImg->setPixel(x, 20+29*canvas->layernum + y,layer_bottom_reverseImg->pixel(x,y));
	}
      }
      if(canvas->currentlayer == 0){
	if(canvas->layernum <= 1){
	  for(int y=0; y<22; y++){
	    for(int x=69; x< 148; x++){
	      layerPanelImg->setPixel(x, 20+29*canvas->layernum + y, layer_bottom_backgroundImg->pixel(x,y));
	    }
	  }
	}
	else{
	  for(int y=0; y<22; y++){
	    for(int x=90; x< 148; x++){
	      layerPanelImg->setPixel(x, 20+29*canvas->layernum + y, layer_bottom_backgroundImg->pixel(x,y));
	    }
	  }
	}
	for(int y=22; y<41; y++){
	  for(int x=0; x< 148; x++){
	    layerPanelImg->setPixel(x, 20+29*canvas->layernum + y, layer_bottom_backgroundImg->pixel(x,y));
	  }
	}

      }
      if(canvas->currentlayer == 1){
	for(int y=0; y<22; y++){
	  for(int x=128; x< 148; x++){
	    layerPanelImg->setPixel(x, 20+29*canvas->layernum + y, layer_bottom_backgroundImg->pixel(x,y));
	  }
	}
      }
      if(canvas->currentlayer == canvas->layernum -1){
	for(int y=0; y<22; y++){
	  for(int x=110; x< 128; x++){
	    layerPanelImg->setPixel(x, 20+29*canvas->layernum + y, layer_bottom_backgroundImg->pixel(x,y));
	  }
	}
      }

      changeOpacityArea(OPACITY_X + canvas->layerAlpha[canvas->currentlayer]);
      QPixmap *layerPix = new QPixmap();
      layerPix->convertFromImage(*layerPanelImg, 0);

      QPainter p;
      p.begin( layerPix );
      p.setFont(QFont("lcfont", 8));

      if(canvas->currentlayer != 0){
	QString tmpStr = "100";
	tmpStr = tmpStr.sprintf("%d",canvas->layerAlpha[canvas->currentlayer]);
	p.setPen( black );
	p.drawText(QRect(OPACITY_X,20+29*canvas->layernum+OPACITY_Y-11, OPACITY_WIDTH-7, 12),Qt::AlignRight , tmpStr);
      }

      if(canvas->currentlayer == 0){
	p.setPen( white );
      }
      else{
	p.setPen( black );
      }
      p.drawText(QRect(29,20+(canvas->layernum-1)*29+8, 55 ,12),Qt::AlignCenter , canvas->layerName[0]);

      for(int i=1; i < canvas->layernum; i++){
	if(canvas->currentlayer == i){
	  p.setPen( white );
	}
	else{
	  p.setPen( black );
	}
	p.drawText(QRect(29,20+(canvas->layernum-1-i)*29+8, 55 ,12),Qt::AlignCenter , canvas->layerName[i]);
      }

      p.end();

      *layerPanelImg = layerPix->convertToImage();
      setBackgroundPixmap(*layerPix);
      delete layerPix;

    }
    else if(!(opacityMinusSelectFlag || opacityPlusSelectFlag)){

      layerPanelImg->setPixel(OPACITY_X+1+canvas->layerAlpha[canvas->currentlayer],20+29*canvas->layernum+OPACITY_Y+1, 0xff9B9B9B);
      layerPanelImg->setPixel(OPACITY_X+1+canvas->layerAlpha[canvas->currentlayer],20+29*canvas->layernum+OPACITY_Y+2, 0xff9B9B9B);
      layerPanelImg->setPixel(OPACITY_X+1+canvas->layerAlpha[canvas->currentlayer],20+29*canvas->layernum+OPACITY_Y+3, 0xff9B9B9B);
      layerPanelImg->setPixel(OPACITY_X+1+canvas->layerAlpha[canvas->currentlayer],20+29*canvas->layernum+OPACITY_Y+4, 0xff9B9B9B);

      layerPanelImg->setPixel(OPACITY_X  +canvas->layerAlpha[canvas->currentlayer],20+29*canvas->layernum+OPACITY_Y+2, 0xff9B9B9B);
      layerPanelImg->setPixel(OPACITY_X  +canvas->layerAlpha[canvas->currentlayer],20+29*canvas->layernum+OPACITY_Y+3, 0xff9B9B9B);
      layerPanelImg->setPixel(OPACITY_X  +canvas->layerAlpha[canvas->currentlayer],20+29*canvas->layernum+OPACITY_Y+4, 0xff9B9B9B);

      layerPanelImg->setPixel(OPACITY_X+2+canvas->layerAlpha[canvas->currentlayer],20+29*canvas->layernum+OPACITY_Y+2, 0xff9B9B9B);
      layerPanelImg->setPixel(OPACITY_X+2+canvas->layerAlpha[canvas->currentlayer],20+29*canvas->layernum+OPACITY_Y+3, 0xff9B9B9B);
      layerPanelImg->setPixel(OPACITY_X+2+canvas->layerAlpha[canvas->currentlayer],20+29*canvas->layernum+OPACITY_Y+4, 0xff9B9B9B);

      changeOpacityArea(e->x());

      for(int x=OPACITY_X+75; x<OPACITY_X+OPACITY_WIDTH-7;x++)
	for(int y=20+29*canvas->layernum+OPACITY_Y-11; y<20+29*canvas->layernum+OPACITY_Y;y++)
	  layerPanelImg->setPixel(x,y,0xff9B9B9B);



      QPixmap *layerPix = new QPixmap();
      layerPix->convertFromImage(*layerPanelImg, 0);

      QPainter p;
      p.begin( layerPix );
      p.setFont(QFont("lcfont", 8));

      if(canvas->currentlayer != 0){
	QString tmpStr = "100";
	tmpStr = tmpStr.sprintf("%d",canvas->layerAlpha[canvas->currentlayer]);
	p.setPen( black );
	p.drawText(QRect(OPACITY_X,20+29*canvas->layernum+OPACITY_Y-11, OPACITY_WIDTH-7, 12),Qt::AlignRight , tmpStr);
      }

      p.end();


      *layerPanelImg = layerPix->convertToImage();
      repaint_left = 0;
      repaint_top = 20+(canvas->layernum)*29+OPACITY_Y-12;
      repaint_width = 148;
      repaint_height = OPACITY_HEIGHT+12;

      canvas->toolbar->repaintLayerOpacity(repaint_left,repaint_top,repaint_width,repaint_height);

      delete layerPix;
    }

  }


}


void Layer::refreshDirectPaintArea(){

  //  usleep(500);
  usleep(700);

}

void Layer::mouseReleaseEvent( QMouseEvent *e ){

  int next_x, next_y;
  bool nocloseFlag = false;
  int tmpOpacity =  OPACITY_X + canvas->layerAlpha[canvas->currentlayer];;

  if(moveFlag){
    next_x = x()+e->x()-prev_x;
    next_y = y()+e->y()-prev_y;
  
    if(next_x < -100){
      next_x = -100;
    }
    else if(next_x > 480-18){
      next_x = 480-18;
    }

    if(next_y < 0){
      next_y = 0;
    }
    else if(next_y > 640-18){
      next_y = 640-18;
    }
    move(next_x, next_y);

    moveFlag = false;
    canvas->toolbar->redrawToolBar();
    return;
  }


  int tmpIndex = canvas->layernum - ((e->y()-20)/29) -1;



  if(e->y() >= 20+29*canvas->layernum + 2  && e->y() < 20+29*canvas->layernum + 21){
    canvas->layerModePanel->hide();

    if(e->x() >= 3 && e->x() <= 21){
      // Create Layer

      if(canvas->layernum >= 5){
	if (!QMessageBox::warning(this, "CloverPaint", "Sorry, over max layer number...\n","OK")){
	  canvas->clover->myResize();
	  return;
	}
	canvas->clover->myResize();
	return;
      }

      canvas->fileChangeFlag = true;


      QImage *tmpImage = new QImage();
      if(!tmpImage->create(SCREEN_WIDTH, SCREEN_HEIGHT, 32,0,QImage::IgnoreEndian)){
	if (!QMessageBox::warning(this, "CloverPaint", "Sorry, no available memory...\n","OK")){
	  canvas->clover->myResize();
	  delete tmpImage;
	  return;
	}
	canvas->clover->myResize();
      }

      if(canvas->currentlayer < canvas->layernum -1){
	for(int i=canvas->layernum-1; i>canvas->currentlayer; i--){
	  canvas->layer[i+1] = canvas->layer[i];
	  canvas->layerMode[i+1] = canvas->layerMode[i];
	  canvas->layerVisible[i+1] = canvas->layerVisible[i];
	  canvas->layerDrawable[i+1] = canvas->layerDrawable[i];
	  canvas->layerName[i+1] = canvas->layerName[i];
	  canvas->layerAlpha[i+1] = canvas->layerAlpha[i];
	  canvas->layerAlpha2[i+1] = canvas->layerAlpha2[i];
	}
      }

      canvas->layer[canvas->currentlayer + 1] = tmpImage;

      canvas->layernum++;

      for(int x=0; x<SCREEN_WIDTH; x++){
	for(int y=0; y<SCREEN_HEIGHT; y++){
	  canvas->layer[canvas->currentlayer + 1]->setPixel(x,y,0x00ffffff);
	}
      }
      canvas->layerMode[canvas->currentlayer + 1] = canvas->previousLayerMode;
      canvas->layerVisible[canvas->currentlayer + 1] = true;
      canvas->layerDrawable[canvas->currentlayer + 1] = true;
      canvas->layerName[canvas->currentlayer + 1] = canvas->layerName[canvas->currentlayer + 1].sprintf("Layer%3d",canvas->layerNameCounter);
      canvas->layerNameCounter++;
      if(canvas->layerNameCounter >= 1000){
	canvas->layerNameCounter = 1;
      }

      canvas->layerAlpha[canvas->currentlayer + 1] = 100;
      canvas->layerAlpha2[canvas->currentlayer + 1] = 256;

      canvas->currentlayer++;
      if(stickyFlag){
	if(this->y() >= 29){
	  move(this->x(), this->y()-29);
	}
      }
      RebuildPanel();
      this->show();  

      for(int i=0; i<SCREEN_WIDTH*SCREEN_HEIGHT; i++){
	*(canvas->undoBuffer+i) = 0x00ffffff;
      }
      for(int i=0; i<SCREEN_WIDTH*SCREEN_HEIGHT; i++){
	*(canvas->drawChecker+i) = 0x00ffffff;
      }
      canvas->undoFlag = false;

      canvas->toolbar->redrawToolBar();
      canvas->repaint();

      if(canvas->lupePanel->isVisible()){
	canvas->lupePanel->myReset();
	canvas->lupePanel->repaint();
      }


      return;
    }
    else if(e->x() >= 25 && e->x() <= 43){
      // Duplicate Layer
      if(canvas->layernum >= 5){
	if (!QMessageBox::warning(this, "CloverPaint", "Sorry, over max layer number...\n","OK")){
	  canvas->clover->myResize();
	  return;
	}
	canvas->clover->myResize();
	return;
      }

      canvas->fileChangeFlag = true;

      QImage *tmpImage = new QImage();
      if(!tmpImage->create(SCREEN_WIDTH, SCREEN_HEIGHT, 32,0,QImage::IgnoreEndian)){
	if (!QMessageBox::warning(this, "CloverPaint", "Sorry, no available memory...\n","OK")){
	  canvas->clover->myResize();
	  delete tmpImage;
	  return;
	}
	canvas->clover->myResize();
      }

      if(canvas->currentlayer < canvas->layernum -1){
	for(int i=canvas->layernum-1; i>canvas->currentlayer; i--){
	  canvas->layer[i+1] = canvas->layer[i];
	  canvas->layerMode[i+1] = canvas->layerMode[i];
	  canvas->layerVisible[i+1] = canvas->layerVisible[i];
	  canvas->layerDrawable[i+1] = canvas->layerDrawable[i];
	  canvas->layerName[i+1] = canvas->layerName[i];

	  canvas->layerAlpha[i + 1] = canvas->layerAlpha[i];
	  canvas->layerAlpha2[i + 1] = canvas->layerAlpha2[i];

	}
      }

      canvas->layer[canvas->currentlayer + 1] = tmpImage;

      canvas->layernum++;

      for(int x=0; x<SCREEN_WIDTH; x++){
	for(int y=0; y<SCREEN_HEIGHT; y++){
	  canvas->layer[canvas->currentlayer + 1]->setPixel(x,y,canvas->layer[canvas->currentlayer]->pixel(x,y));
	}
      }
      canvas->layerMode[canvas->currentlayer + 1] = canvas->layerMode[canvas->currentlayer];
      canvas->layerVisible[canvas->currentlayer + 1] = canvas->layerVisible[canvas->currentlayer];
      canvas->layerDrawable[canvas->currentlayer + 1] = canvas->layerDrawable[canvas->currentlayer];
      canvas->layerName[canvas->currentlayer + 1] = canvas->layerName[canvas->currentlayer + 1].sprintf("Layer%3d",canvas->layerNameCounter);
      canvas->layerNameCounter++;
      if(canvas->layerNameCounter >= 1000){
	canvas->layerNameCounter = 1;
      }
      canvas->layerAlpha[canvas->currentlayer + 1] = canvas->layerAlpha[canvas->currentlayer];
      canvas->layerAlpha2[canvas->currentlayer + 1] = canvas->layerAlpha2[canvas->currentlayer];

      canvas->currentlayer++;
      if(stickyFlag){
	if(this->y() >= 29){
	  move(this->x(), this->y()-29);
	}
      }
      RebuildPanel();
      this->show();  

      for(int i=0; i<SCREEN_WIDTH*SCREEN_HEIGHT; i++){
	*(canvas->undoBuffer+i) = 0x00ffffff;
      }
      for(int i=0; i<SCREEN_WIDTH*SCREEN_HEIGHT; i++){
	*(canvas->drawChecker+i) = 0x00ffffff;
      }

      canvas->undoFlag = false;

      canvas->toolbar->redrawToolBar();
      canvas->repaint();

      if(canvas->lupePanel->isVisible()){
	canvas->lupePanel->myReset();
	canvas->lupePanel->repaint();
      }


      return;

    }
    else if(e->x() >= 47 && e->x() <= 65){
      // Erase Layer

      canvas->fileChangeFlag = true;


      for(int y=0; y<640; y++){
	for(int x=0; x<480; x++){
	  *(canvas->undoBuffer+x+y*480) = canvas->layer[canvas->currentlayer]->pixel(x,y);
	}
      }
      for(int i=0; i<SCREEN_WIDTH*SCREEN_HEIGHT; i++){
	*(canvas->drawChecker+i) = 0x00ffffff;
      }

      canvas->undoFlag = false;
      canvas->toolbar->redrawToolBar();

      if(canvas->currentlayer == 0){
	for(int x=0; x<SCREEN_WIDTH; x++){
	  for(int y=0; y<SCREEN_HEIGHT; y++){
	    canvas->layer[canvas->currentlayer]->setPixel(x,y,0xffffffff);
	  }
	}
      }
      else{
	for(int x=0; x<SCREEN_WIDTH; x++){
	  for(int y=0; y<SCREEN_HEIGHT; y++){
	    canvas->layer[canvas->currentlayer]->setPixel(x,y,0x00ffffff);
	  }
	}
      }
      canvas->repaint();

      if(canvas->lupePanel->isVisible()){
	canvas->lupePanel->myReset();
	canvas->lupePanel->repaint();
      }

      canvas->undo_left = 0;
      canvas->undo_top = 0;
      canvas->undo_right = 479;
      canvas->undo_bottom = 639;

      if(canvas->zoomStyle == LUPE){
	canvas->dispLupeFlag = true;
	canvas->lupePanel->hide();
      }

    }
    else if(e->x() >= 69 && e->x() <= 87){
      // Merge Layer

      if(canvas->layernum <= 1){
	return;
      }
      if (!QMessageBox::warning(this, "CloverPaint",
				"Join visible layers? (cannot undo)\n",
				"OK", "Cancel")){
	canvas->fileChangeFlag = true;

	int mergeBaseLayer = -1;

	for(int i=canvas->layernum-1; i>=0 ; i--){
	  if(canvas->layerVisible[i]){
	    mergeBaseLayer = i;
	  }
	}

	if(mergeBaseLayer == -1){
	  canvas->clover->myResize();
	  canvas->toolbar->redrawToolBar();
	  return;
	}
	
	// 背景が非表示の場合は
	// 下のレイヤを白地に対して合成した後、上のレイヤと合成する．
	// (Photoshopとは異なる挙動)
	unsigned int whiteColor = 0xffffffff;
	unsigned int blackColor = 0xff000000;

	int deletedNum = 0;

	for(int m=mergeBaseLayer+1; m<canvas->layernum; m++){
	  if(canvas->layerVisible[m]){
	    deletedNum++;
	    //		fprintf(stderr,"delete %d\n",deletedNum);
	    for(int x=0; x<SCREEN_WIDTH; x++){
	      for(int y=0; y<SCREEN_HEIGHT; y++){
		// 下のレイヤのアルファ
		int alpha1 = ((canvas->layer[mergeBaseLayer]->pixel(x,y) >> 24) & 0x000000ff);
		// 上のレイヤのアルファ
		int alpha2 = ((canvas->layer[m]->pixel(x,y) >> 24) & 0x000000ff);
		//合成後のアルファ
		
		int newAlpha = 0;
		if(canvas->layerMode[m] == HUE
		   || canvas->layerMode[m] == SATURATION
		   || canvas->layerMode[m] == COLOR
		   || canvas->layerMode[m] == BRIGHTNESS
		   || canvas->layerMode[m] == BURN
		   || canvas->layerMode[m] == DODGE
		   ){
		  newAlpha = alpha1;
		}
		else{
		  newAlpha = ((((256-alpha1)*alpha2)>>8)&0x000000ff) + alpha1;
		}

		// 下のレイヤの白地あるいは黒地に対するみかけの色
		unsigned int lowerColor = 0;
		if(canvas->layerMode[mergeBaseLayer] == SCREEN
		   || canvas->layerMode[mergeBaseLayer] == LIGHTEN
		   || canvas->layerMode[mergeBaseLayer] == DIFFERENT){
		  lowerColor = canvas->calc(canvas->layerMode[mergeBaseLayer], blackColor, canvas->layer[mergeBaseLayer]->pixel(x,y), canvas->layerAlpha2[mergeBaseLayer]);
		}
		else if(canvas->layerMode[mergeBaseLayer] == HUE
			|| canvas->layerMode[mergeBaseLayer] == SATURATION
			|| canvas->layerMode[mergeBaseLayer] == COLOR
			|| canvas->layerMode[mergeBaseLayer] == BRIGHTNESS
			|| canvas->layerMode[mergeBaseLayer] == BURN
			|| canvas->layerMode[mergeBaseLayer] == DODGE
			){
		  lowerColor = canvas->calc(NORMAL, whiteColor, canvas->layer[mergeBaseLayer]->pixel(x,y), canvas->layerAlpha2[mergeBaseLayer]);
		}
		else{
		  lowerColor = canvas->calc(canvas->layerMode[mergeBaseLayer], whiteColor, canvas->layer[mergeBaseLayer]->pixel(x,y), canvas->layerAlpha2[mergeBaseLayer]);
		}
		// みかけの色に上のレイヤを合成. 見た目の色が求まる．
		unsigned int viewColor = 0;
		if(canvas->layerMode[mergeBaseLayer] == HUE
		   || canvas->layerMode[mergeBaseLayer] == SATURATION
		   || canvas->layerMode[mergeBaseLayer] == COLOR
		   || canvas->layerMode[mergeBaseLayer] == BRIGHTNESS
		   || canvas->layerMode[mergeBaseLayer] == BURN
		   || canvas->layerMode[mergeBaseLayer] == DODGE
		   ){
		  viewColor = canvas->calc(NORMAL, lowerColor, canvas->layer[m]->pixel(x,y), canvas->layerAlpha2[m]);
		}
		else{
		  viewColor = canvas->calc(canvas->layerMode[m], lowerColor, canvas->layer[m]->pixel(x,y), canvas->layerAlpha2[m]);
		}
		// 新たなレイヤがノーマル合成である場合に、合成後のアルファによって見た目の色をもたらすための実際の色を逆演算する．

		int realR = 255;
		int realG = 255;
		int realB = 255;
		if(newAlpha != 0){
		  realR = ((255-((viewColor >> 16)&0x000000ff))<<8)/newAlpha;
		  if(realR <= 0){
		    realR = 0;
		  }
		  else if(realR >=255){
		    realR = 255;
		  }
		  realR = 255-realR;

		  realG = ((255-((viewColor >> 8)&0x000000ff))<<8)/newAlpha;
		  if(realG <= 0){
		    realG = 0;
		  }
		  else if(realG >=255){
		    realG = 255;
		  }
		  realG = 255-realG;

		  realB = ((255-((viewColor)&0x000000ff))<<8)/newAlpha;
		  if(realB <= 0){
		    realB = 0;
		  }
		  else if(realB >=255){
		    realB = 255;
		  }
		  realB = 255-realB;
		}

		unsigned int realColor = ((newAlpha << 24)&0xff000000) | ((realR << 16)&0x00ff0000) | ((realG << 8)&0x0000ff00)  | ((realB )&0x000000ff);
		canvas->layer[mergeBaseLayer]->setPixel(x,y, realColor);

	      }
	    }
	  }
	}
	canvas->layerAlpha[mergeBaseLayer] = 100;
	canvas->layerAlpha2[mergeBaseLayer] = 256;

	for(int i=mergeBaseLayer+1; i<canvas->layernum; i++){
	  if(canvas->layerVisible[i]){
	    if(canvas->layerName[i] != "moved"){
	      delete canvas->layer[i];
	    }

	    for(int j=i+1; j<canvas->layernum; j++){
	      if(!canvas->layerVisible[j]){
		canvas->layer[i] = canvas->layer[j];
		canvas->layerMode[i] = canvas->layerMode[j];
		canvas->layerVisible[i] = canvas->layerVisible[j];
		canvas->layerDrawable[i] = canvas->layerDrawable[j];
		canvas->layerName[i] = canvas->layerName[j];
		canvas->layerAlpha[i] = canvas->layerAlpha[j];
		canvas->layerAlpha2[i] = canvas->layerAlpha2[j];

		// 名前を変え消す対象に含める
		canvas->layerVisible[j] = true;
		canvas->layerDrawable[j] = true;
		canvas->layerName[j] = "moved";
		break;
	      }
	    }


	  }
	}

	canvas->layernum = canvas->layernum - deletedNum;

	canvas->currentlayer = mergeBaseLayer;
	canvas->clover->myResize();
	RebuildPanel();
	this->show();


	for(int y=0; y<640; y++){
	  for(int x=0; x<480; x++){
	    *(canvas->undoBuffer+x+y*480) = canvas->layer[canvas->currentlayer]->pixel(x,y);
	  }
	}

	for(int i=0; i<SCREEN_WIDTH*SCREEN_HEIGHT; i++){
	  *(canvas->drawChecker+i) = 0x00ffffff;
	}

	canvas->undoFlag = false;

      }
      else{
	canvas->clover->myResize();
	RebuildPanel();
      }

      canvas->toolbar->redrawToolBar();

      if(canvas->lupePanel->isVisible()){
	canvas->lupePanel->myReset();
	canvas->lupePanel->repaint();
      }



      return;

    }
    else if(e->x() >= 90 && e->x() <= 108 && canvas->currentlayer != 0){
      // Delete Layer
      if(canvas->layernum <= 1){
	return;
      }
      if(!canvas->layerVisible[canvas->currentlayer]){
	return;
      }

      if (!QMessageBox::warning(this, "CloverPaint",
				"Delete this layer? (cannot undo)\n",
				"OK", "Cancel")){
	canvas->fileChangeFlag = true;


	delete canvas->layer[canvas->currentlayer];

	for(int i=canvas->currentlayer; i<canvas->layernum-1; i++){
	  canvas->layer[i] = canvas->layer[i+1];
	  canvas->layerMode[i] = canvas->layerMode[i+1];
	  canvas->layerVisible[i] = canvas->layerVisible[i+1];
	  canvas->layerDrawable[i] = canvas->layerDrawable[i+1];
	  canvas->layerName[i] = canvas->layerName[i+1];
	  canvas->layerAlpha[i] = canvas->layerAlpha[i+1];
	  canvas->layerAlpha2[i] = canvas->layerAlpha2[i+1];

	}


	canvas->layernum--;
	if(canvas->currentlayer != 0){
	  canvas->currentlayer--;
	}

	canvas->layerMode[0] = NORMAL;

	canvas->clover->myResize();
	RebuildPanel();
	this->show();  

	for(int y=0; y<640; y++){
	  for(int x=0; x<480; x++){
	    *(canvas->undoBuffer+x+y*480) = canvas->layer[canvas->currentlayer]->pixel(x,y);
	  }
	}
	for(int i=0; i<SCREEN_WIDTH*SCREEN_HEIGHT; i++){
	  *(canvas->drawChecker+i) = 0x00ffffff;
	}
	canvas->undoFlag = false;
	canvas->toolbar->redrawToolBar();

	if(canvas->lupePanel->isVisible()){
	  canvas->lupePanel->myReset();
	  canvas->lupePanel->repaint();
	}

	return;
      }
      else{
	canvas->clover->myResize();
	RebuildPanel();
	return;
      }

    }
    else if(e->x() >= 112 && e->x() <= 126 && canvas->currentlayer != 0){
      // Up Layer

      if(canvas->currentlayer == canvas->layernum-1){
	return;
      }

      canvas->fileChangeFlag = true;

      
      QImage *tmpImage;
      short tmpMode;
      int tmpAlpha;
      int tmpAlpha2;
      bool tmpVisible;
      bool tmpDrawable;

      QString tmpName;
      tmpImage = canvas->layer[canvas->currentlayer+1];
      tmpMode = canvas->layerMode[canvas->currentlayer+1];
      tmpVisible = canvas->layerVisible[canvas->currentlayer+1];
      tmpDrawable = canvas->layerDrawable[canvas->currentlayer+1];
      tmpName = canvas->layerName[canvas->currentlayer+1];
      tmpAlpha = canvas->layerAlpha[canvas->currentlayer+1];
      tmpAlpha2 = canvas->layerAlpha2[canvas->currentlayer+1];

      canvas->layer[canvas->currentlayer+1] = canvas->layer[canvas->currentlayer];
      canvas->layerMode[canvas->currentlayer+1] = canvas->layerMode[canvas->currentlayer];
      canvas->layerVisible[canvas->currentlayer+1] = canvas->layerVisible[canvas->currentlayer];
      canvas->layerDrawable[canvas->currentlayer+1] = canvas->layerDrawable[canvas->currentlayer];
      canvas->layerName[canvas->currentlayer+1] =  canvas->layerName[canvas->currentlayer];
      canvas->layerAlpha[canvas->currentlayer+1] = canvas->layerAlpha[canvas->currentlayer];
      canvas->layerAlpha2[canvas->currentlayer+1] = canvas->layerAlpha2[canvas->currentlayer];

      canvas->layer[canvas->currentlayer] = tmpImage;
      canvas->layerMode[canvas->currentlayer] = tmpMode;
      canvas->layerVisible[canvas->currentlayer] = tmpVisible;
      canvas->layerDrawable[canvas->currentlayer] = tmpDrawable;
      canvas->layerName[canvas->currentlayer] = tmpName;

      canvas->layerAlpha[canvas->currentlayer] = tmpAlpha;
      canvas->layerAlpha2[canvas->currentlayer] = tmpAlpha2;

      canvas->currentlayer++;

      canvas->layerMode[0] = NORMAL;

      RebuildPanel();
      this->show();  
      canvas->toolbar->redrawToolBar();
      canvas->repaint();
      if(canvas->lupePanel->isVisible()){
	canvas->lupePanel->myReset();
	canvas->lupePanel->repaint();
      }

      return;


    }
    else if(e->x() >= 130 && e->x() <= 144 && canvas->currentlayer > 1){
      // Down Layer

      canvas->fileChangeFlag = true;


      QImage *tmpImage;
      short tmpMode;
      int tmpAlpha;
      int tmpAlpha2;
      bool tmpVisible;
      bool tmpDrawable;

      QString tmpName;
      tmpImage = canvas->layer[canvas->currentlayer-1];
      tmpMode = canvas->layerMode[canvas->currentlayer-1];
      tmpVisible = canvas->layerVisible[canvas->currentlayer-1];
      tmpDrawable = canvas->layerDrawable[canvas->currentlayer-1];
      tmpName = canvas->layerName[canvas->currentlayer-1];
      tmpAlpha = canvas->layerAlpha[canvas->currentlayer-1];
      tmpAlpha2 = canvas->layerAlpha2[canvas->currentlayer-1];

      canvas->layer[canvas->currentlayer-1] = canvas->layer[canvas->currentlayer];
      canvas->layerMode[canvas->currentlayer-1] = canvas->layerMode[canvas->currentlayer];
      canvas->layerVisible[canvas->currentlayer-1] = canvas->layerVisible[canvas->currentlayer];
      canvas->layerDrawable[canvas->currentlayer-1] = canvas->layerDrawable[canvas->currentlayer];
      canvas->layerName[canvas->currentlayer-1] = canvas->layerName[canvas->currentlayer];
      canvas->layerAlpha[canvas->currentlayer-1] = canvas->layerAlpha[canvas->currentlayer];
      canvas->layerAlpha2[canvas->currentlayer-1] = canvas->layerAlpha2[canvas->currentlayer];

      canvas->layer[canvas->currentlayer] = tmpImage;
      canvas->layerMode[canvas->currentlayer] = tmpMode;
      canvas->layerVisible[canvas->currentlayer] = tmpVisible;
      canvas->layerDrawable[canvas->currentlayer] = tmpDrawable;
      canvas->layerName[canvas->currentlayer] = tmpName;

      canvas->layerAlpha[canvas->currentlayer] = tmpAlpha;
      canvas->layerAlpha2[canvas->currentlayer] = tmpAlpha2;

      canvas->currentlayer--;

      canvas->layerMode[0] = NORMAL;

      RebuildPanel();
      this->show();  
      canvas->toolbar->redrawToolBar();
      canvas->repaint();
      if(canvas->lupePanel->isVisible()){
	canvas->lupePanel->myReset();
	canvas->lupePanel->repaint();
      }

      return;

    }


    // オートクローズ無効
    nocloseFlag = true;
    
  }
  else if(e->y() >= 20+29*canvas->layernum + 21  && canvas->currentlayer != 0){
    layerPanelImg->setPixel(OPACITY_X+1+canvas->layerAlpha[canvas->currentlayer],20+29*canvas->layernum+OPACITY_Y+1, 0xff9B9B9B);
    layerPanelImg->setPixel(OPACITY_X+1+canvas->layerAlpha[canvas->currentlayer],20+29*canvas->layernum+OPACITY_Y+2, 0xff9B9B9B);
    layerPanelImg->setPixel(OPACITY_X+1+canvas->layerAlpha[canvas->currentlayer],20+29*canvas->layernum+OPACITY_Y+3, 0xff9B9B9B);
    layerPanelImg->setPixel(OPACITY_X+1+canvas->layerAlpha[canvas->currentlayer],20+29*canvas->layernum+OPACITY_Y+4, 0xff9B9B9B);

    layerPanelImg->setPixel(OPACITY_X  +canvas->layerAlpha[canvas->currentlayer],20+29*canvas->layernum+OPACITY_Y+2, 0xff9B9B9B);
    layerPanelImg->setPixel(OPACITY_X  +canvas->layerAlpha[canvas->currentlayer],20+29*canvas->layernum+OPACITY_Y+3, 0xff9B9B9B);
    layerPanelImg->setPixel(OPACITY_X  +canvas->layerAlpha[canvas->currentlayer],20+29*canvas->layernum+OPACITY_Y+4, 0xff9B9B9B);

    layerPanelImg->setPixel(OPACITY_X+2+canvas->layerAlpha[canvas->currentlayer],20+29*canvas->layernum+OPACITY_Y+2, 0xff9B9B9B);
    layerPanelImg->setPixel(OPACITY_X+2+canvas->layerAlpha[canvas->currentlayer],20+29*canvas->layernum+OPACITY_Y+3, 0xff9B9B9B);
    layerPanelImg->setPixel(OPACITY_X+2+canvas->layerAlpha[canvas->currentlayer],20+29*canvas->layernum+OPACITY_Y+4, 0xff9B9B9B);


    if(e->x() <= 17 && !opacitySelectingFlag){
      // Minus Opacity
      canvas->fileChangeFlag = true;

      tmpOpacity = OPACITY_X + canvas->layerAlpha[canvas->currentlayer]-1;

      if(tmpOpacity< OPACITY_X + 0){
	tmpOpacity = OPACITY_X + 0;
      }
      else if(tmpOpacity > OPACITY_X + 100){
	tmpOpacity = OPACITY_X + 100;
      }
      changeOpacityArea(tmpOpacity);
      canvas->repaint();
    }
    else if(e->x() >= 129 && !opacitySelectingFlag){
      // Plus Opacity
      canvas->fileChangeFlag = true;
      tmpOpacity = OPACITY_X + canvas->layerAlpha[canvas->currentlayer]+1;

      if(tmpOpacity< OPACITY_X + 0){
	tmpOpacity = OPACITY_X + 0;
      }
      else if(tmpOpacity > OPACITY_X + 100){
	tmpOpacity = OPACITY_X + 100;
      }
      changeOpacityArea(tmpOpacity);
      canvas->repaint();
    }
    else if(!(opacityMinusSelectFlag || opacityPlusSelectFlag)){
      canvas->fileChangeFlag = true;

      tmpOpacity = e->x();

      for(int x=OPACITY_X+75; x<OPACITY_X+OPACITY_WIDTH-7;x++)
	for(int y=20+29*canvas->layernum+OPACITY_Y-11; y<20+29*canvas->layernum+OPACITY_Y;y++)
	  layerPanelImg->setPixel(x,y,0xff9B9B9B);
      changeOpacityArea(tmpOpacity);
      canvas->repaint();
    }

    canvas->lupePanel->myReset();
    nocloseFlag = true;

  }


  if(stickyFlag){
    for(int y=3; y<17; y++){
      for(int x=115; x<144; x++){
	layerPanelImg->setPixel(x,y,layer_top_reverseImg->pixel(x,y));
      }
    }
  }
  else{
    for(int y=3; y<17; y++){
      for(int x=115; x<144; x++){
	layerPanelImg->setPixel(x,y,layer_top_normalImg->pixel(x,y));
      }
    }
  }


  if(e->y() >= 20  && e->y() < 20+29*canvas->layernum && e->x() >= 82 && e->x() <=143 ){

    if( canvas->currentlayer == tmpIndex && canvas->layerModePanel->isVisible()){
      canvas->layerModePanel->hide();

      if(tmpIndex == 0){
	for(int y=0; y<21; y++){
	  for(int x=0; x<62; x++){
	    layer_normalImg->setPixel(x+82,y+4,layer_nomodeImg->pixel(x,y));
	  }
	}
      }
      else{
	for(int y=0; y<21; y++){
	  for(int x=0; x<62; x++){
	    layer_normalImg->setPixel(x+82,y+4,canvas->mode_normalImg->pixel(x+1,y+1+canvas->layerMode[canvas->currentlayer]*21));
	  }
	}
      }

      if(canvas->layerVisible[canvas->currentlayer]){
	for(int y=0; y<29; y++){
	  for(int x=0; x<18; x++){
	    layer_normalImg->setPixel(x,y,layer_visibleImg->pixel(x,y));
	  }
	}
      }
      else{
	for(int y=0; y<29; y++){
	  for(int x=0; x<18; x++){
	    layer_normalImg->setPixel(x,y,layer_invisibleImg->pixel(x,y));
	  }
	}
      }


      if(canvas->currentlayer == 0){
	for(int y=0; y<29; y++){
	  for(int x=19; x<34; x++){
	    layer_normalImg->setPixel(x,y,layer_normalImgOrg->pixel(x,y));
	  }
	}
      }
      else if(canvas->layerDrawable[canvas->currentlayer]){
	for(int y=0; y<29; y++){
	  for(int x=19; x<34; x++){
	    layer_normalImg->setPixel(x,y,layer_freeImg->pixel(x-19,y));
	  }
	}
      }
      else{
	for(int y=0; y<29; y++){
	  for(int x=19; x<34; x++){
	    layer_normalImg->setPixel(x,y,layer_hogoImg->pixel(x-19,y));
	  }
	}
      }



      for(int y=0; y<29; y++){
	for(int x=1; x<1+145; x++){
	  if(x >= 82 && x<=143 && y>=4 && y <=24 && canvas->currentlayer != 0){
	    layerPanelImg->setPixel(x,y+20+(canvas->layernum-canvas->currentlayer-1)*29,layer_normalImg->pixel(x,y));
	  }
	  else{
	    layerPanelImg->setPixel(x,y+20+(canvas->layernum-canvas->currentlayer-1)*29,(~(layer_normalImg->pixel(x,y))) | 0xff000000);
	  }
	}
      }

      changeOpacityArea(OPACITY_X + canvas->layerAlpha[canvas->currentlayer]);


      QPixmap *layerPix = new QPixmap();
      layerPix->convertFromImage(*layerPanelImg, 0);
      
      QPainter p;
      p.begin( layerPix );
      p.setFont(QFont("lcfont", 8));

      if(canvas->currentlayer != 0){
	QString tmpStr = "100";
	tmpStr = tmpStr.sprintf("%d",canvas->layerAlpha[canvas->currentlayer]);
	p.setPen( black );
	p.drawText(QRect(OPACITY_X,20+29*canvas->layernum+OPACITY_Y-11, OPACITY_WIDTH-7, 12),Qt::AlignRight , tmpStr);
      }

      if(canvas->currentlayer == 0){
	p.setPen( white );
      }
      else{
	p.setPen( black );
      }
      p.drawText(QRect(29,20+(canvas->layernum-1)*29+8, 55 ,12),Qt::AlignCenter , canvas->layerName[0]);

      for(int i=1; i < canvas->layernum; i++){
	if(canvas->currentlayer == i){
	  p.setPen( white );
	}
	else{
	  p.setPen( black );
	}
	p.drawText(QRect(29,20+(canvas->layernum-1-i)*29+8, 55 ,12),Qt::AlignCenter , canvas->layerName[i]);
      }

      p.end();

      setBackgroundPixmap(*layerPix);
      delete layerPix;

      canvas->lupePanel->myReset();

      return;
    }
    else{
      canvas->layerModePanel->hide();      
    }
  }

  int prevLayer = canvas->currentlayer;

  if(e->y() >= 20  && e->y() < 20+29*canvas->layernum){

    if(e->x() >= 0 && e->x() <=18){
      int tmpLayerNum = canvas->layernum - ((e->y()-20)/29) -1;
      canvas->layerVisible[tmpLayerNum] = !canvas->layerVisible[tmpLayerNum];
      RebuildPanel();
      this->show();  
      canvas->repaint();

      if(canvas->lupePanel->isVisible()){
	canvas->lupePanel->myReset();
	canvas->lupePanel->repaint();
      }
    }
    else if(e->x() >= 19 && e->x() <=34){
      int tmpLayerNum = canvas->layernum - ((e->y()-20)/29) -1;
      canvas->layerDrawable[tmpLayerNum] = !canvas->layerDrawable[tmpLayerNum];
      RebuildPanel();
      this->show();  
      canvas->repaint();

      if(canvas->lupePanel->isVisible()){
	canvas->lupePanel->myReset();
	canvas->lupePanel->repaint();
      }
    }
    else{
      if(canvas->currentlayer != canvas->layernum - ((e->y()-20)/29) -1){
	canvas->currentlayer = canvas->layernum - ((e->y()-20)/29) -1;
	for(int y=0; y<640; y++){
	  for(int x=0; x<480; x++){
	    *(canvas->undoBuffer+x+y*480) = canvas->layer[canvas->currentlayer]->pixel(x,y);
	  }
	}
	for(int i=0; i<SCREEN_WIDTH*SCREEN_HEIGHT; i++){
	  *(canvas->drawChecker+i) = 0x00ffffff;
	}
	canvas->undoFlag = false;
      }
      else{
	canvas->currentlayer = canvas->layernum - ((e->y()-20)/29) -1;
      }

      canvas->toolbar->redrawToolBar();
      tmpOpacity =  OPACITY_X + canvas->layerAlpha[canvas->currentlayer];
    }
  }


  for(int y=0; y<41; y++){
    for(int x=0; x< 148; x++){
      layerPanelImg->setPixel(x, 20+29*canvas->layernum + y,layer_bottom_normalImg->pixel(x,y));
    }
  }

  if(canvas->currentlayer == 0){
    if(canvas->layernum <= 1){
      for(int y=0; y<22; y++){
	for(int x=69; x< 148; x++){
	  layerPanelImg->setPixel(x, 20+29*canvas->layernum + y, layer_bottom_backgroundImg->pixel(x,y));
	}
      }
    }
    else{
      for(int y=0; y<22; y++){
	for(int x=90; x< 148; x++){
	  layerPanelImg->setPixel(x, 20+29*canvas->layernum + y, layer_bottom_backgroundImg->pixel(x,y));
	}
      }
    }
    for(int y=22; y<41; y++){
      for(int x=0; x< 148; x++){
	layerPanelImg->setPixel(x, 20+29*canvas->layernum + y, layer_bottom_backgroundImg->pixel(x,y));
      }
    }

  }

  if(canvas->currentlayer == 1){
    for(int y=0; y<22; y++){
      for(int x=128; x< 148; x++){
	layerPanelImg->setPixel(x, 20+29*canvas->layernum + y, layer_bottom_backgroundImg->pixel(x,y));
      }
    }
  }
  if(canvas->currentlayer == canvas->layernum -1){
    for(int y=0; y<22; y++){
      for(int x=110; x< 128; x++){
	layerPanelImg->setPixel(x, 20+29*canvas->layernum + y, layer_bottom_backgroundImg->pixel(x,y));
      }
    }
  }



  if(canvas->currentlayer != 0){
    changeOpacityArea(tmpOpacity);    
  }

  QPixmap *layerPix = new QPixmap();
  layerPix->convertFromImage(*layerPanelImg, 0);

  QPainter p;
  p.begin( layerPix );
  p.setFont(QFont("lcfont", 8));

  if(canvas->currentlayer != 0){
    QString tmpStr = "100";
    tmpStr = tmpStr.sprintf("%d",canvas->layerAlpha[canvas->currentlayer]);
    p.setPen( black );
    p.drawText(QRect(OPACITY_X,20+29*canvas->layernum+OPACITY_Y-11, OPACITY_WIDTH-7, 12),Qt::AlignRight , tmpStr);
  }

  if(canvas->currentlayer == 0){
    p.setPen( white );
  }
  else{
    p.setPen( black );
  }
  p.drawText(QRect(29,20+(canvas->layernum-1)*29+8, 55 ,12),Qt::AlignCenter , canvas->layerName[0]);

  for(int i=1; i < canvas->layernum; i++){
    if(canvas->currentlayer == i){
      p.setPen( white );
    }
    else{
      p.setPen( black );
    }


    p.drawText(QRect(29,20+(canvas->layernum-1-i)*29+8, 55 ,12),Qt::AlignCenter , canvas->layerName[i]);
  }

  p.end();

  setBackgroundPixmap(*layerPix);
  delete layerPix;

  canvas->lupePanel->myReset();

  if(e->x() >= 82 && e->x() <=143 && e->y()>20 && e->y()<20+(canvas->layernum)*29 && canvas->currentlayer != 0){

    //モードパネルを開く
    int tmpLeft = this->x() + 148;
    int tmpTop = this->y() +20 + (canvas->layernum - canvas->currentlayer-1)*29 + 4 - canvas->layerMode[canvas->currentlayer] * 21;
    if(tmpLeft > 480-65){
      tmpLeft = this->x()-65;
    }
    if(tmpTop > 640-276){
      tmpTop = 640-276;
    }
    if(tmpTop < 0){
      tmpTop = 0;
    }
    canvas->layerModePanel->move(tmpLeft, tmpTop);
    canvas->layerModePanel->show();

    return;
  }
  else{
    canvas->layerModePanel->hide();
  }


  //スティッキー
  if(e->y() >= 0 && e->y() < 17){
    if(e->x() >= 115 && e->x() < 129){
      stickyFlag = true;
      //      refreshDirectPaintArea();
      this->hide();
      display = false;
    }
    else if(e->x() >= 130 && e->x() < 144){
      stickyFlag = false;
      //      refreshDirectPaintArea();
      this->hide();
      display = false;
      this->move(LAYERPANEL_X,canvas->layerPanelY);

    }
  }

  if(!stickyFlag && !nocloseFlag){
    //    refreshDirectPaintArea();
    this->hide();
      display = false;
  }


}
