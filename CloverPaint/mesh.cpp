#include "clover.h"

#define MESHICON_X 331
#define MESHICON_Y 2
#define MESHICON_WIDTH 29
#define MESHICON_HEIGHT 29

Mesh::Mesh(Canvas * parent, const char * name, bool modal, WFlags f)
  :QDialog(parent, name, modal, f)
{
  canvas = parent;

  moveFlag = false;
  stickyFlag = false;
  minimizePressFlag = false;
  closePressFlag = false;
  display = false;
  reparent( parent,  QPoint(MESHPANEL_X, canvas->meshPanelY));
  resize(65, 201);

  //xpmからの読みこみはインデックスカラーになるため変換
  meshImg = new QImage(*(canvas->mesh_normalImg));

  //ツールバー画像の表示
  QPixmap *meshPix = new QPixmap();
  meshPix->convertFromImage(*meshImg, 0);
  setBackgroundPixmap(*meshPix);
  delete meshPix;

}

Mesh::~Mesh()
{
  delete meshImg;
}

void Mesh::showEvent( QShowEvent *e ){
  this->setActiveWindow();
  this->raise();

  if(!stickyFlag && (this->y() != canvas->meshPanelY)){
    move(MESHPANEL_X, canvas->meshPanelY);
  }

  changeMesh(-1, -1);
}

void Mesh::hideEvent( QHideEvent *e ){

}

void Mesh::paintEvent( QPaintEvent *e ){
  // 他のダイアログとデッドロックを起こすので以下は利用しないこと

  //changeMesh(-1, -1);
}

void Mesh::changeMesh(int mesh, int overwrite){

  int left=0;
  int top=0;

  int left2=0;
  int top2=0;
  
  if(mesh == -1){
    mesh = canvas->meshpattern;
  }
  else{
    canvas->meshpattern = mesh;
  }

  switch(mesh){
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
  

  if(overwrite == -1){
    overwrite = canvas->overwrite;
  }
  else{
    canvas->overwrite = overwrite;
  }

  switch(overwrite){
  case NORMAL_DRAW:
    left2 = 32;
    top2 = 168;
    break;
  case OVERWRITE:
    left2 = 3;
    top2 = 168;	  
    break;
  default:
    break;
  }


  *meshImg = canvas->mesh_normalImg->copy();

  for(int y=top; y<top+29; y++){
    for(int x=left; x<left+29; x++){
      meshImg->setPixel(x,y,canvas->mesh_reverseImg->pixel(x,y));
    }
  }

  for(int y=top2; y<top2+29; y++){
    for(int x=left2; x<left2+29; x++){
      meshImg->setPixel(x,y,canvas->mesh_reverseImg->pixel(x,y));
    }
  }

  if(stickyFlag){
    for(int y=3; y<17; y++){
      for(int x=32; x<61; x++){
	meshImg->setPixel(x,y,canvas->mesh_reverseImg->pixel(x,y));
      }
    }
  }
  else{
    for(int y=3; y<17; y++){
      for(int x=32; x<61; x++){
	meshImg->setPixel(x,y,canvas->mesh_normalImg->pixel(x,y));
      }
    }
  }


    if(minimizePressFlag){
      for(int y=3; y<17; y++){
	for(int x=32; x<46; x++){
	  meshImg->setPixel(x,y,(~(canvas->mesh_reverseImg->pixel(x,y))) | 0xff000000);
	}
      }
    }
    else{
      if(stickyFlag){
	for(int y=3; y<17; y++){
	  for(int x=32; x<46; x++){
	    meshImg->setPixel(x,y,((canvas->mesh_reverseImg->pixel(x,y))) | 0xff000000);
	  }
	}
      }
      else{
	for(int y=3; y<17; y++){
	  for(int x=32; x<46; x++){
	    meshImg->setPixel(x,y,((canvas->mesh_normalImg->pixel(x,y))) | 0xff000000);
	  }
	}
      }
    }

    if(closePressFlag){
      for(int y=3; y<17; y++){
	for(int x=47; x<61; x++){
	  meshImg->setPixel(x,y,(~(canvas->mesh_reverseImg->pixel(x,y))) | 0xff000000);
	}
      }
    }
    else{
      if(stickyFlag){
	for(int y=3; y<17; y++){
	  for(int x=47; x<61; x++){
	    meshImg->setPixel(x,y,((canvas->mesh_reverseImg->pixel(x,y))) | 0xff000000);
	  }
	}
      }
      else{
	for(int y=3; y<17; y++){
	  for(int x=47; x<61; x++){
	    meshImg->setPixel(x,y,((canvas->mesh_normalImg->pixel(x,y))) | 0xff000000);
	  }
	}
      }
    }


  QPixmap *meshPix = new QPixmap();
  meshPix->convertFromImage(*meshImg, 0);
  setBackgroundPixmap(*meshPix);
  delete meshPix;

}


void Mesh::mousePressEvent( QMouseEvent *e )
{

  minimizePressFlag = false;
  closePressFlag = false;

  if(!this->isTopLevel()){
    this->setActiveWindow();
    this->raise();
  }

  if(e->y() >= 0 && e->y() < 17){
    if(e->x() >= 0 && e->x() < 32){

      prev_x = e->x();
      prev_y = e->y();

      moveFlag = true;
      stickyFlag = true;

      changeMesh(-1, -1);
      return;
    }
  }

  if(e->y() >= 0 && e->y() < 17){
    if(e->x() >= 32 && e->x() < 46){
      minimizePressFlag = true;
      changeMesh(-1, -1);
      return;
    }
    else if(e->x() >= 47 && e->x() < 61){
      closePressFlag = true;
      changeMesh(-1, -1);
      return;
    }
  }


  if(e->x() >= 3 && e->x() < 32){
    // 左列
    if(e->y() >= 20 && e->y() < 49){
      changeMesh(MY_LIGHT_GRAY, -1);
    }
    else if(e->y() >= 49 && e->y() < 78){
      changeMesh(MY_LEFT_INCLINE, -1);
    }
    else if(e->y() >= 78 && e->y()< 107){
      changeMesh(MY_RIGHT_INCLINE, -1);
    }
    else if(e->y() >= 107 && e->y()< 136){
      changeMesh(MY_BORDER, -1);
    }
    else if(e->y() >= 136 && e->y()< 165){
      changeMesh(MY_STRIPE, -1);
    }
    else if(e->y() >= 168 && e->y()< 197){
      changeMesh(-1, OVERWRITE);
    }

  }
  else if (e->x()>= 32 && e->x()< 61){
    // 右列
    if(e->y() >= 20 && e->y() < 49){
      changeMesh(MY_BLACK, -1);
    }
    else if(e->y() >= 49 && e->y() < 78){
      changeMesh(MY_DARK_GRAY, -1);
    }
    else if(e->y() >= 78 && e->y()< 107){
      changeMesh(MY_GRAY, -1);
    }    
    else if(e->y() >= 107 && e->y()< 136){
      changeMesh(MY_FENCE_GRAY, -1);
    }    
    else if(e->y() >= 136 && e->y()< 165){
      changeMesh(MY_GRID_GRAY, -1);
    }    
    else if(e->y() >= 168 && e->y()< 197){
      changeMesh(-1, NORMAL_DRAW);
    }    

  }

}


void Mesh::mouseMoveEvent( QMouseEvent *e )
{

  int next_x, next_y;
  minimizePressFlag = false;
  closePressFlag = false;

  if(moveFlag){
    next_x = x()+e->x()-prev_x;
    next_y = y()+e->y()-prev_y;
  
    if(next_x < 47-65){
      next_x = 47-65;
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

    return;
  }

  if(e->y() >= 0 && e->y() < 17){
    if(e->x() >= 32 && e->x() < 46){
      minimizePressFlag = true;
      changeMesh(-1, -1);
      return;
    }
    else if(e->x() >= 47 && e->x() < 61){
      closePressFlag = true;
      changeMesh(-1, -1);
      return;
    }
    else{
      minimizePressFlag = false;
      closePressFlag = false;
      changeMesh(-1, -1);
      return;
    }
  }


  if(e->x() >= 3 && e->x() < 32){
    // 左列
    if(e->y() >= 20 && e->y() < 49){
      changeMesh(MY_LIGHT_GRAY, -1);
    }
    else if(e->y() >= 49 && e->y() < 78){
      changeMesh(MY_LEFT_INCLINE, -1);
    }
    else if(e->y() >= 78 && e->y()< 107){
      changeMesh(MY_RIGHT_INCLINE, -1);
    }
    else if(e->y() >= 107 && e->y()< 136){
      changeMesh(MY_BORDER, -1);
    }
    else if(e->y() >= 136 && e->y()< 165){
      changeMesh(MY_STRIPE, -1);
    }
    else if(e->y() >= 168 && e->y()< 197){
      changeMesh(-1, OVERWRITE);
    }

  }
  else if (e->x()>= 32 && e->x()< 61){
    // 右列
    if(e->y() >= 20 && e->y() < 49){
      changeMesh(MY_BLACK, -1);
    }
    else if(e->y() >= 49 && e->y() < 78){
      changeMesh(MY_DARK_GRAY, -1);
    }
    else if(e->y() >= 78 && e->y()< 107){
      changeMesh(MY_GRAY, -1);
    }    
    else if(e->y() >= 107 && e->y()< 136){
      changeMesh(MY_FENCE_GRAY, -1);
    }    
    else if(e->y() >= 136 && e->y()< 165){
      changeMesh(MY_GRID_GRAY, -1);
    }    
    else if(e->y() >= 168 && e->y()< 197){
      changeMesh(-1, NORMAL_DRAW);
    }    

  }


}



void Mesh::mouseReleaseEvent( QMouseEvent *e ){


  int next_x, next_y;
  minimizePressFlag = false;
  closePressFlag = false;

  if(moveFlag){
    next_x = x()+e->x()-prev_x;
    next_y = y()+e->y()-prev_y;
  
    if(next_x < 47-65){
      next_x = 47-65;
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


  //スティッキー
  if(e->x() >= 51){
    if(e->y() < 13){
      if(stickyFlag){
	stickyFlag = false;
      }
      else{
	stickyFlag = true;
      }

    }
  }


  //スティッキー
  if(e->y() >= 0 && e->y() < 17){
    if(e->x() >= 32 && e->x() < 46){
      stickyFlag = true;
      this->hide();
      display = false;
    }
    else if(e->x() >= 47 && e->x() < 61){
      stickyFlag = false;
      this->hide();
      display = false;
      this->move(MESHPANEL_X,canvas->meshPanelY);
    }
  }

  if(!stickyFlag){
     this->hide();
      display = false;
  }

  changeMesh(-1, -1);

  canvas->toolbar->redrawToolBar();

}
