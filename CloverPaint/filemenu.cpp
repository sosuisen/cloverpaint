#include "clover.h"
#include "filemenu_normal.h"
#include "filemenu_reverse.h"



FileMenu::FileMenu(Canvas * parent, const char * name, bool modal, WFlags f)
  :QDialog(parent, name, modal, f)
{
  canvas = parent;

  reparent( parent, QPoint(this->x(), this->y()));
  resize(80, 255);
  this->move(16,640 - 31 - 20 - this->height());


  //画像の読み込み
  QImage tmpImg = QImage(filemenu_normal);
  //xpmからの読みこみはインデックスカラーになるため変換
  filemenuImg = tmpImg.convertDepth(32);
  filemenu_normalImg = tmpImg.convertDepth(32);

  //画像(裏)の読み込み
  tmpImg = QImage(filemenu_reverse);
  filemenu_reverseImg = tmpImg.convertDepth(32);

  //画像の表示
  QPixmap filemenuPix = QPixmap();
  filemenuPix.convertFromImage(filemenu_normalImg, 0);
  setBackgroundPixmap(filemenuPix);


}

FileMenu::~FileMenu()
{
}


void FileMenu::showEvent( QShowEvent *e ){
  this->setActiveWindow();
  this->raise();

  if((this->y() != canvas->fileMenuPanelY)){
    move(this->x(), canvas->fileMenuPanelY);
  }


  for(int y=0; y<this->height(); y++){
    for(int x=1; x<this->width(); x++){
      filemenuImg.setPixel(x,y,filemenu_normalImg.pixel(x,y));
    }
  }

  QPixmap filemenuPix = QPixmap();
  filemenuPix.convertFromImage(filemenuImg, 0);
  setBackgroundPixmap(filemenuPix);


}

void FileMenu::hideEvent( QHideEvent *e ){

}


void FileMenu::mousePressEvent( QMouseEvent *e )
{

  if(e->y()<= 0 || e->y() >= this->height()-1){
    return;
  }

  int tmpIndex = e->y()/21;

  if(tmpIndex > 11){
    tmpIndex = 11;
  }

  for(int y=0; y<this->height(); y++){
    for(int x=1; x<this->width(); x++){
      filemenuImg.setPixel(x,y,filemenu_normalImg.pixel(x,y));
    }
  }

  for(int y=0; y<21; y++){
      for(int x=0; x<this->width(); x++){
	filemenuImg.setPixel(x,y+tmpIndex*21,filemenu_reverseImg.pixel(x,y+tmpIndex*21));
      }
  }

  QPixmap filemenuPix = QPixmap();
  filemenuPix.convertFromImage(filemenuImg, 0);
  setBackgroundPixmap(filemenuPix);

}


void FileMenu::mouseMoveEvent( QMouseEvent *e )
{

  if(e->y()<= 0 || e->y() >= this->height()-1){
    return;
  }

  int tmpIndex = e->y()/21;

  if(tmpIndex > 11){
    tmpIndex = 11;
  }

  for(int y=0; y<this->height(); y++){
    for(int x=1; x<this->width(); x++){
      filemenuImg.setPixel(x,y,filemenu_normalImg.pixel(x,y));
    }
  }

  for(int y=0; y<21; y++){
      for(int x=0; x<this->width(); x++){
	filemenuImg.setPixel(x,y+tmpIndex*21,filemenu_reverseImg.pixel(x,y+tmpIndex*21));
      }
  }

  QPixmap filemenuPix = QPixmap();
  filemenuPix.convertFromImage(filemenuImg, 0);
  setBackgroundPixmap(filemenuPix);
}



void FileMenu::mouseReleaseEvent( QMouseEvent *e ){

  int tmpTop=0;
  int tmpLeft=0;

  if(e->y()<= 0 || e->y() >= this->height()-1){
    return;
  }

  int tmpIndex = e->y()/21;

  if(tmpIndex > 11){
    tmpIndex = 11;
  }

  canvas->toolbar->hide();
  canvas->huebar->hide();
  canvas->pensizePanel->hide();
  canvas->meshPanel->hide();
  canvas->palettePanel->hide();
  canvas->scalePanel->hide();
  canvas->toolPanel->hide();
  if(canvas->zoomStyle == LUPE){
    canvas->dispLupeFlag = true;
    canvas->lupePanel->hide();
  }


  canvas->layerPanel->hide();
  canvas->layerModePanel->hide();
  canvas->fileMenuPanel->hide();  
  canvas->selectPanel->hide();  



  QRegion reg;
  QRegion r;

  switch(tmpIndex){
  case 0:
    // New
    if(canvas->fileChangeFlag){
      canvas->dispLupeFlag = false;
      if (QMessageBox::warning(this, "CloverPaint",
			     "Changes are not saved.\nProceed anyway?\n",
			     "OK", "Cancel")){
	canvas->clover->myResize();
	break;
      }
    }
    canvas->dispLupeFlag = false;

    canvas->canvasType = VGA;
    canvas->zoomScale = 0; //シフト数で指定。 0 は等倍．
    canvas->lastScale = 1; //最後に選択された等倍以上の拡大率
    canvas->lupeScale = 2; //シフト数で指定。 0 は等倍．
    canvas->zoomStyle = NORMAL;
    canvas->canvasTop = 0;
    canvas->canvasLeft = 0;
    canvas->zoomTop = 120;
    canvas->zoomLeft = 160;
    canvas->lupeTop = 210 ;
    canvas->lupeLeft = 130;

    canvas->lupePanel->changeWidget();

    for(int i=1; i<canvas->layernum; i++){
      delete canvas->layer[i];
    }
    canvas->layernum = 1;
    canvas->currentlayer = 0;

    for(int y=0; y<640; y++){
      for(int x=0; x<480; x++){
	canvas->layer[0]->setPixel(x,y,0xffffffff);
	canvas->bgImage->setPixel(x,y,0xffffffff);
      }
    }
    canvas->layerMode[0] = NORMAL;
    canvas->layerVisible[0] = true;
    canvas->layerDrawable[0] = true;
    canvas->layerAlpha[0] = 100;
    canvas->layerAlpha2[0] = 255;
    canvas->layerNameCounter = 1;

    canvas->undo_left = 480;
    canvas->undo_top = 640;
    canvas->undo_right = 480;
    canvas->undo_bottom = 640;

    canvas->repaint();
    canvas->layerPanel->RebuildPanel();
    
    canvas->clover->myResize();

    for(int i=0; i<SCREEN_WIDTH*SCREEN_HEIGHT; i++){
      *(canvas->undoBuffer+i) = 0xffffffff;
    }
    for(int i=0; i<SCREEN_WIDTH*SCREEN_HEIGHT; i++){
      *(canvas->drawChecker+i) = 0x00ffffff;
    }

    canvas->undoFlag = false;

    canvas->currentFilename =  "";

    
    break;

  case 1:
    // New QVGA
    if(canvas->fileChangeFlag){
      canvas->dispLupeFlag = false;
      if (QMessageBox::warning(this, "CloverPaint",
			     "Changes are not saved.\nProceed anyway?\n",
			     "OK", "Cancel")){
	canvas->clover->myResize();
	break;
      }
    }
    canvas->dispLupeFlag = false;

    canvas->canvasType = QVGA;

    canvas->zoomScale = 1;
    canvas->lupeScale = 2;
    canvas->zoomStyle = ZOOM;
    canvas->lastScale = 2; //最後に選択された等倍以上の拡大率
    canvas->canvasTop = 0;
    canvas->canvasLeft = 0;
    canvas->zoomTop = 0;
    canvas->zoomLeft = 0;
    canvas->lupeTop = 105+22;
    canvas->lupeLeft = 65+22;

    canvas->lupePanel->changeWidget();

    for(int i=1; i<canvas->layernum; i++){
      delete canvas->layer[i];
    }
    canvas->layernum = 1;
    canvas->currentlayer = 0;

    for(int y=0; y<640; y++){
      for(int x=0; x<480; x++){
	canvas->layer[0]->setPixel(x,y,0xffffffff);
	canvas->bgImage->setPixel(x,y,0xffffffff);
      }
    }
    canvas->layerMode[0] = NORMAL;
    canvas->layerVisible[0] = true;
    canvas->layerDrawable[0] = true;
    canvas->layerAlpha[0] = 100;
    canvas->layerAlpha2[0] = 255;
    canvas->layerNameCounter = 1;

    canvas->undo_left = 480;
    canvas->undo_top = 640;
    canvas->undo_right = 480;
    canvas->undo_bottom = 640;

    canvas->repaint();
    canvas->layerPanel->RebuildPanel();
    
    canvas->clover->myResize();

    for(int i=0; i<SCREEN_WIDTH*SCREEN_HEIGHT; i++){
      *(canvas->undoBuffer+i) = 0xffffffff;
    }
    for(int i=0; i<SCREEN_WIDTH*SCREEN_HEIGHT; i++){
      *(canvas->drawChecker+i) = 0x00ffffff;
    }

    canvas->undoFlag = false;

    canvas->currentFilename =  "";


    break;

  case 2:
    // Open ...
    canvas->clover->openImage();

    canvas->lupePanel->changeWidget();

    canvas->clover->myResize();

    for(int y=0; y<640; y++){
      for(int x=0; x<480; x++){
	*(canvas->undoBuffer+x+y*480) = canvas->layer[canvas->currentlayer]->pixel(x,y);
      }
    }
    canvas->undoFlag = false;

    break;

  case 3:
    // Save ...
    tmpTop = canvas->canvasTop;
    tmpLeft = canvas->canvasLeft;
    if(canvas->zoomStyle == ZOOM){
      canvas->zoomStyle = NORMAL;
      canvas->repaint();

      canvas->clover->saveImage();
      canvas->zoomStyle = ZOOM;
    }
    else{
      canvas->repaint();
      canvas->clover->saveImage();
    }
    canvas->canvasTop = tmpTop;
    canvas->canvasLeft = tmpLeft;
    canvas->clover->myResize();
    break;

  case 4:
    // Save as
    tmpTop = canvas->canvasTop;
    tmpLeft = canvas->canvasLeft;
    if(canvas->zoomStyle == ZOOM){
      canvas->zoomStyle = NORMAL;
      canvas->repaint();
      canvas->clover->saveCpfImage();
      canvas->zoomStyle = ZOOM;
    }
    else{
      canvas->repaint();
      canvas->clover->saveCpfImage();
    }
    canvas->canvasTop = tmpTop;
    canvas->canvasLeft = tmpLeft;
    canvas->clover->myResize();
    break;

  case 5:
    // Save PNG
    tmpTop = canvas->canvasTop;
    tmpLeft = canvas->canvasLeft;
    if(canvas->zoomStyle == ZOOM){
      canvas->zoomStyle = NORMAL;
      canvas->repaint();
      canvas->clover->savePngImage();
      canvas->zoomStyle = ZOOM;
    }
    else{
      canvas->repaint();
      canvas->clover->savePngImage();
    }
    canvas->canvasTop = tmpTop;
    canvas->canvasLeft = tmpLeft;
    canvas->clover->myResize();
    break;

  case 6:
    // Save JPG
    tmpTop = canvas->canvasTop;
    tmpLeft = canvas->canvasLeft;
    if(canvas->zoomStyle == ZOOM){
      canvas->zoomStyle = NORMAL;
      canvas->repaint();
      canvas->clover->saveJpgImage();
      canvas->zoomStyle = ZOOM;
    }
    else{
      canvas->repaint();
      canvas->clover->saveJpgImage();
    }
    canvas->canvasTop = tmpTop;
    canvas->canvasLeft = tmpLeft;
    canvas->clover->myResize();
    break;


  case 7:
    // Load Layer
    // ここにfprintfなどでウェイトを入れるとうまく動作する
    // 非同期的動作
    canvas->clover->loadLayer();

    //        canvas->clover->myResize();

    canvas->clover->reparent( NULL, WStyle_StaysOnTop | WStyle_Customize | WStyle_NoBorderEx, QPoint(0, 0));
    canvas->clover->showNormal();
    canvas->clover->showFullScreen();


    for(int y=0; y<640; y++){
      for(int x=0; x<480; x++){
	*(canvas->undoBuffer+x+y*480) = canvas->layer[canvas->currentlayer]->pixel(x,y);
      }
    }

    canvas->undoFlag = false;

    break;

  case 8:
    // Save Layer
    canvas->clover->saveLayer();

    break;

  case 9:
    // ColorAdjust
    
    if (canvas->useColorMatrix){
      canvas->useColorMatrix = false;
      //      QMessageBox::about(canvas->clover, "CloverPaint", "Color Adjust OFF");
    }
    else{
      canvas->useColorMatrix = true;
      //      QMessageBox::about(canvas->clover, "CloverPaint", "Color Adjust ON");
    }
    canvas->clover->myResize();

    canvas->toolbar->changeSBArea();
    canvas->toolbar->changeColorArea();
    
    canvas->toolbar->repaint_left = SBAREA_X-2;
    canvas->toolbar->repaint_top = SBAREA_Y-2;
    canvas->toolbar->repaint_width = SBAREA_WIDTH+4;
    canvas->toolbar->repaint_height = SBAREA_HEIGHT+4;
    r = QRegion(canvas->toolbar->repaint_left,canvas->toolbar->repaint_top,canvas->toolbar->repaint_width,canvas->toolbar->repaint_height,QRegion::Rectangle);
    canvas->toolbar->repaint(r, false);

    canvas->toolbar->repaint_left = COLORAREA_X;
    canvas->toolbar->repaint_top = COLORAREA_Y;
    canvas->toolbar->repaint_width = COLORAREA_WIDTH;
    canvas->toolbar->repaint_height = COLORAREA_HEIGHT;
    r =QRegion(canvas->toolbar->repaint_left,canvas->toolbar->repaint_top,canvas->toolbar->repaint_width,canvas->toolbar->repaint_height,QRegion::Rectangle);
    canvas->toolbar->repaint(r, false);


    canvas->toolbar->repaint_left = COLORAREA3_X;
    canvas->toolbar->repaint_top = COLORAREA3_Y;
    canvas->toolbar->repaint_width = COLORAREA3_WIDTH;
    canvas->toolbar->repaint_height = COLORAREA3_HEIGHT;
    r =QRegion(canvas->toolbar->repaint_left,canvas->toolbar->repaint_top,canvas->toolbar->repaint_width,canvas->toolbar->repaint_height,QRegion::Rectangle);
    canvas->toolbar->repaint(r, false);

    break;


  case 10:
    // About
    QMessageBox::about(canvas->clover, "CloverPaint", "CloverPaint version 1.1\nDeveloped by Juro Soga\n<VFF04555@nifty.ne.jp>");
    canvas->clover->myResize();
    break;

  case 11:
    // Exit
    canvas->clover->close();
    canvas->clover->myResize();
    break;

  default:
    break;
  }

  canvas->changeToolbarVisible();
  canvas->toolbar->redrawToolBar();


  hide();



}
