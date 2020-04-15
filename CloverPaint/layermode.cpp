#include "clover.h"

LayerMode::LayerMode(Canvas * parent, const char * name, bool modal, WFlags f)
  :QDialog(parent, name, modal, f)
{
  canvas = parent;

  reparent( parent, QPoint(this->x(), this->y()));
  resize(65, 276);


  //ツールバー画像の表示
  modeImg = canvas->mode_normalImg->copy();
  QPixmap modePix = QPixmap();
  modePix.convertFromImage(*(canvas->mode_normalImg), 0);
  setBackgroundPixmap(modePix);

}

LayerMode::~LayerMode()
{
}


void LayerMode::showEvent( QShowEvent *e ){
  this->setActiveWindow();
  this->raise();

  for(int y=0; y<276; y++){
    for(int x=1; x<65; x++){
      modeImg.setPixel(x,y,canvas->mode_normalImg->pixel(x,y));
    }
  }
  

  for(int y=0; y<22; y++){
    for(int x=1; x<65; x++){
      modeImg.setPixel(x,y+1+21*canvas->layerMode[canvas->currentlayer],canvas->mode_reverseImg->pixel(x,y+1+21*canvas->layerMode[canvas->currentlayer]));
    }
  }
  QPixmap modePix = QPixmap();
  modePix.convertFromImage(modeImg, 0);
  setBackgroundPixmap(modePix);

}

void LayerMode::hideEvent( QHideEvent *e ){

}


void LayerMode::mousePressEvent( QMouseEvent *e )
{

  int tmpIndex = e->y()/21;

  if(tmpIndex > 12){
    tmpIndex = 12;
  }
  else if(tmpIndex < 0){
    tmpIndex = 0;
  }

  for(int y=0; y<276; y++){
    for(int x=1; x<65; x++){
      modeImg.setPixel(x,y,canvas->mode_normalImg->pixel(x,y));
    }
  }

  for(int y=0; y<21; y++){
      for(int x=0; x<65; x++){
	modeImg.setPixel(x,y+tmpIndex*21,canvas->mode_reverseImg->pixel(x,y+tmpIndex*21));
      }
  }

  QPixmap modePix = QPixmap();
  modePix.convertFromImage(modeImg, 0);
  setBackgroundPixmap(modePix);

}


void LayerMode::mouseMoveEvent( QMouseEvent *e )
{
  int tmpIndex = e->y()/21;

  if(tmpIndex > 12){
    tmpIndex = 12;
  }
  else if(tmpIndex < 0){
    tmpIndex = 0;
  }

  for(int y=0; y<276; y++){
    for(int x=1; x<65; x++){
      modeImg.setPixel(x,y,canvas->mode_normalImg->pixel(x,y));
    }
  }

  for(int y=0; y<21; y++){
      for(int x=0; x<65; x++){
	modeImg.setPixel(x,y+tmpIndex*21,canvas->mode_reverseImg->pixel(x,y+tmpIndex*21));
      }
  }

  QPixmap modePix = QPixmap();
  modePix.convertFromImage(modeImg, 0);
  setBackgroundPixmap(modePix);

}



void LayerMode::mouseReleaseEvent( QMouseEvent *e ){

  int tmpIndex = e->y()/21;
  if(tmpIndex > 12){
    tmpIndex = 12;
  }
  else if(tmpIndex < 0){
    tmpIndex = 0;
  }

  canvas->fileChangeFlag = true;

  canvas->layerMode[canvas->currentlayer] = tmpIndex;
  canvas->previousLayerMode = tmpIndex;

  hide();
  canvas->layerPanel->RebuildPanel();
  canvas->repaint();
  canvas->lupePanel->myReset();

}
