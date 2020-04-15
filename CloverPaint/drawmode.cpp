#include "clover.h"

DrawMode::DrawMode(Canvas * parent, const char * name, bool modal, WFlags f)
  :QDialog(parent, name, modal, f)
{
  canvas = parent;

  reparent( parent, QPoint(DRAWMODEPANEL_X, 640-23-20-this->height()));
  resize(65, 276);

  //ツールバー画像の表示
  QPixmap modePix = QPixmap();
  modeImg = canvas->mode_normalImg->copy();
  modePix.convertFromImage(*canvas->mode_normalImg, 0);
  setBackgroundPixmap(modePix);

}

DrawMode::~DrawMode()
{
}


void DrawMode::showEvent( QShowEvent *e ){
  if(this->y() != canvas->drawModePanelY){
    move(this->x(), canvas->drawModePanelY);
  }

  this->setActiveWindow();
  this->raise();


  for(int y=0; y<276; y++){
    for(int x=1; x<65; x++){
      modeImg.setPixel(x,y,canvas->mode_normalImg->pixel(x,y));
    }
  }
  

  for(int y=0; y<22; y++){
    for(int x=1; x<65; x++){
      modeImg.setPixel(x,y+1+21*canvas->drawMode,canvas->mode_reverseImg->pixel(x,y+1+21*canvas->drawMode));
    }
  }
  QPixmap modePix = QPixmap();
  modePix.convertFromImage(modeImg, 0);
  setBackgroundPixmap(modePix);

}

void DrawMode::hideEvent( QHideEvent *e ){

}


void DrawMode::mousePressEvent( QMouseEvent *e )
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


void DrawMode::mouseMoveEvent( QMouseEvent *e )
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



void DrawMode::mouseReleaseEvent( QMouseEvent *e ){


  int tmpIndex = e->y()/21;
  if(tmpIndex > 12){
    tmpIndex = 12;
  }
  else if(tmpIndex < 0){
    tmpIndex = 0;
  }

  canvas->fileChangeFlag = true;

  canvas->drawMode = tmpIndex;

  hide();

  canvas->toolbar->redrawToolBar();  

}
