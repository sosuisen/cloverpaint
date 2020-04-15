#include "clover.h"
#include <sl/slfiledialog.h>
#include <sl/slfileselector.h>
#include <qdir.h>
#include <qdatetime.h> 
#include <qregexp.h>
#include <qpe/qcopenvelope_qws.h>
#include <math.h>
#include "clovericon.h"

CloverPaint::CloverPaint()
{
  // QDirectPainter dp(this) つまりトップレベルにあるQMainWindowの
  // フレームバッファには描けないので，
  // canvasという子ウィジェットを作成してそこからフレームバッファを取得

  canvas = new Canvas(this);
  setCentralWidget(canvas);

  QImage *tmpImg = new QImage(clovericon);
  QPixmap *iconPix = new QPixmap();
  iconPix->convertFromImage(tmpImg->convertDepth(32));
  setIcon(*iconPix);

  delete tmpImg;
  delete iconPix;



  //スクリーンセーバー抑制
  //  QCopEnvelope disableScreenSaver( "QPE/System", "setScreenSaverIntervals(int,int,int)" );
  //  disableScreenSaver << 0 << 0 << 0;
  //  QCopEnvelope enableScreenSaver( "QPE/System", "setScreenSaverIntervals(int,int,int)" );
  //  enableScreenSaver << -1 << -1 << -1;
    //以下のやりかたでは抑制できない
    //  QCopEnvelope setScreenSaverMode( "QPE/System", "setScreenSaverMode(int)" );
    //  setScreenSaverMode << 0;


}

CloverPaint::~CloverPaint() {

  delete canvas;
}




void CloverPaint::resizeEvent(QResizeEvent *e)
{
  // 全画面モードへ
  // showFullScreen()はコンストラクタに書いても無効
  
  if(QApplication::desktop()->width() == 640 && canvas->windowDirection == TATE){

    for(int y=0; y<640; y++){
      for(int x=0; x<480; x++){
	canvas->bgImage->setPixel(x,y, 0xff000000);
      }
    }
    for(int y=0; y<40; y++){
      for(int x=0; x<29; x++){
	canvas->bgImage->setPixel(305+x,260+y, canvas->cloverImg->pixel(x,y));
      }
    }

    QPixmap canvasPix = QPixmap();
    canvasPix.convertFromImage(*(canvas->bgImage),0);
    QPainter p;
    p.begin(&canvasPix );
    p.setPen( white );
    p.setFont(QFont("lcfont", 20));
    p.drawText(QRect(200,180, 320, 24),Qt::AlignLeft , "CloverPaint:");
    p.drawText(QRect(200,210, 320, 24),Qt::AlignLeft , "Please change to ViewStyle.");
    p.end();  
    canvas->setBackgroundPixmap(canvasPix);

    // 横画面の場合
    if(canvas->toolbar->isVisible()){
      canvas->changeToolbarVisible();
      canvas->lupePanel->hide();
    }

    canvas->windowDirection = YOKO;
    setFixedSize(QApplication::desktop()->size());

    return;
  }
  else if(QApplication::desktop()->width() == 480 && canvas->windowDirection == YOKO){
    canvas->windowDirection = TATE;
    
    if(!canvas->toolbar->isVisible()){
      canvas->changeToolbarVisible();
    }
    if(canvas->zoomStyle == LUPE){
      canvas->lupePanel->show();
    }

    for(int y=0; y<640; y++){
      for(int x=0; x<480; x++){
	canvas->bgImage->setPixel(x,y, 0xffffffff);
      }
    }
    QPixmap canvasPix = QPixmap();
    canvasPix.convertFromImage(*(canvas->bgImage),0);
    canvas->setBackgroundPixmap(canvasPix);

  }

  if ( size() == QApplication::desktop()->size() ) {
    return;
  }

  setFixedSize(QApplication::desktop()->size());

  reparent( NULL, WStyle_StaysOnTop | WStyle_Customize | WStyle_NoBorderEx, QPoint(0, 0));
  showNormal();
  showFullScreen();


}

void CloverPaint::keyReleaseEvent( QKeyEvent *e )
{
  if(canvas->windowDirection == YOKO){
    return;
  }


  switch(e->key()){
  case Qt::Key_Down:
    //    canvas->spoitSelectFlag = false;
    //    canvas->spoitJogSelectFlag = false;
    //    canvas->spoitJogReleaseFlag = true;
    //    canvas->handFlag = false;

    //    canvas->toolbar->redrawToolBar();
    break;
  }
}


void CloverPaint::keyPressEvent( QKeyEvent *e )
{
  //fprintf(stderr, "%x\n",e->key());
  // *拡大       2010
  //calendar 0x1038
  //address  0x1039
  //mail     0x103c
  //home     0x103b
  //縮小 0x200f
  //menu 103a
  //tab 1001

  if(canvas->windowDirection == YOKO){
    switch(e->key()){
    case Qt::Key_Escape:
      close();
      resize();
      break;
    }
    return;
  }



  short *toolsize = NULL;
  switch(canvas->tool){
  case PEN:
    toolsize = &(canvas->pensize);
    break;
  case ERASER:
    toolsize = &(canvas->erasersize);
    break;
  }

  int repaint_left;
  int repaint_top;
  int repaint_width;
  int repaint_height;

  switch(e->key()){
  case Qt::Key_O:
    //    openImage();
    break;

  case 0x1004: // OK
    //    canvas->bgImage->save("spoit.png","PNG");
    //    canvas->fillFlag = true;

    if(canvas->huebarY ==0){
      canvas->toolbarY = TOOLBAR_Y1;
      canvas->huebarY = HUEBAR_Y1;
      canvas->pensizePanelY = PENSIZEPANEL_Y1;
      canvas->meshPanelY = MESHPANEL_Y1;
      canvas->palettePanelY = PALETTEPANEL_Y1;
      canvas->scalePanelY = SCALEPANEL_Y1;
      canvas->toolPanelY = TOOLPANEL_Y1;
      canvas->selectPanelY = 640-31-20-canvas->selectPanel->height();
      canvas->toolbar->move(0, canvas->toolbarY);
      canvas->huebar->move(0, canvas->huebarY);
      canvas->layerPanelY = 640 - 80 - (44+20+29*canvas->layernum);
      canvas->fileMenuPanelY = 640 - 31 - 20 - canvas->fileMenuPanel->height();
      canvas->drawModePanelY = 640 - 23 - 20 - canvas->drawModePanel->height();
    }
    else{
      canvas->toolbarY = TOOLBAR_Y2;
      canvas->huebarY = HUEBAR_Y2;
      canvas->pensizePanelY = PENSIZEPANEL_Y2;
      canvas->meshPanelY = MESHPANEL_Y2;
      canvas->palettePanelY = PALETTEPANEL_Y2;
      canvas->scalePanelY = SCALEPANEL_Y2;
      canvas->toolPanelY = TOOLPANEL_Y2;
      canvas->selectPanelY = SELECTPANEL_Y2;
      canvas->toolbar->move(0, canvas->toolbarY);
      canvas->huebar->move(0, canvas->huebarY);
      canvas->layerPanelY = LAYERPANEL_Y2;
      canvas->fileMenuPanelY = FILEMENUPANEL_Y2;
      canvas->drawModePanelY = DRAWMODEPANEL_Y2;
    }

    if(!canvas->pensizePanel->stickyFlag && canvas->pensizePanel->isVisible()){
      canvas->pensizePanel->hide();
      canvas->pensizePanel->display = false;
    }
    if(!canvas->meshPanel->stickyFlag && canvas->meshPanel->isVisible()){
      canvas->meshPanel->hide();
      canvas->meshPanel->display = false;
    }
    if(!canvas->palettePanel->stickyFlag && canvas->palettePanel->isVisible()){
      canvas->palettePanel->hide();
      canvas->palettePanel->display = false;
    }
    if(!canvas->scalePanel->stickyFlag && canvas->scalePanel->isVisible()){
      canvas->scalePanel->hide();
      canvas->scalePanel->display = false;
    }
    if(!canvas->toolPanel->stickyFlag && canvas->toolPanel->isVisible()){
      canvas->toolPanel->hide();
      canvas->toolPanel->display = false;
    }
    if(!canvas->selectPanel->stickyFlag && canvas->selectPanel->isVisible()){
      canvas->selectPanel->hide();
      canvas->selectPanel->display = false;
    }

    if(canvas->drawModePanel->isVisible()){
      canvas->drawModePanel->hide();
      //      canvas->drawModePanel->display = false;
    }

    if(!canvas->layerPanel->stickyFlag && canvas->layerPanel->isVisible()){
      canvas->layerPanel->hide();
      canvas->layerPanel->display = false;
    }
    if(canvas->fileMenuPanel->isVisible()){
      canvas->fileMenuPanel->hide();
      //      canvas->fileMenuPanel->display = false;
    }

    canvas->toolbar->redrawToolBar();  

    repaint_left = 479-(int)(479*canvas->fgHu/COLORDIV)-1;
    if(repaint_left < 0){
      repaint_left = 0;
    }
    else if(repaint_left > 477){
      repaint_left = 477;
    }
    repaint_top = 0;
    repaint_width = 3;
    repaint_height = 20;
    canvas->toolbar->repaintColor(repaint_left, repaint_top, repaint_width, repaint_height, false);


    break;
  case Qt::Key_S:
    //    saveBmpImage();
    //    saveCpfImage();
    break;

  case Qt::Key_Up:
    if(canvas->zoomStyle != LUPE){
      canvas->handFlag = !canvas->handFlag;
      canvas->spoitSelectFlag = false;
      canvas->toolbar->redrawToolBar();
    }
    break;

  case Qt::Key_Down:
    canvas->spoitSelectFlag = !canvas->spoitSelectFlag;
    canvas->handFlag = false;
    canvas->toolbar->redrawToolBar();

    break;
  case Qt::Key_Escape:
    close();
    resize();
    break;
  case Qt::Key_Right:
    /*    if(canvas->zoomStyle == NORMAL){
      canvas->canvasLeft += SCREEN_WIDTH/2;
      if(canvas->canvasLeft > SCREEN_WIDTH - SCREEN_WIDTH/2)
	canvas->canvasLeft = SCREEN_WIDTH - SCREEN_WIDTH/2;
    canvas->repaint();
    }
    else if(canvas->zoomStyle == ZOOM){
      if(canvas->zoomScale == 1){
	canvas->zoomLeft += SCREEN_WIDTH/2/4;
	if(canvas->zoomLeft > SCREEN_WIDTH - SCREEN_WIDTH/2/2)
	  canvas->zoomLeft = SCREEN_WIDTH - SCREEN_WIDTH/2/2;
      }
      else if(canvas->zoomScale == 2){
	canvas->zoomLeft += SCREEN_WIDTH/2/8;
	if(canvas->zoomLeft > SCREEN_WIDTH - SCREEN_WIDTH/2/4)
	  canvas->zoomLeft = SCREEN_WIDTH - SCREEN_WIDTH/2/4;
      }
      else if(canvas->zoomScale == 3){
	canvas->zoomLeft += SCREEN_WIDTH/2/16;
	if(canvas->zoomLeft > SCREEN_WIDTH - SCREEN_WIDTH/2/8)
	  canvas->zoomLeft = SCREEN_WIDTH - SCREEN_WIDTH/2/8;
      }
      else if(canvas->zoomScale == 4){
	canvas->zoomLeft += SCREEN_WIDTH/2/32;
	if(canvas->zoomLeft > SCREEN_WIDTH - SCREEN_WIDTH/2/16)
	  canvas->zoomLeft = SCREEN_WIDTH - SCREEN_WIDTH/2/16;
      }
    canvas->repaint();
    }
    else if(canvas->zoomStyle == LUPE){
      if(canvas->lupeScale == 1){
	canvas->lupeLeft += LUPE_WIDTH/2/4;
	if(canvas->lupeLeft > SCREEN_WIDTH - LUPE_WIDTH/2/2)
	  canvas->lupeLeft = SCREEN_WIDTH - LUPE_WIDTH/2/2;
      }
      else if(canvas->lupeScale == 2){
	canvas->lupeLeft += LUPE_WIDTH/2/8;
	if(canvas->lupeLeft > SCREEN_WIDTH - LUPE_WIDTH/2/4)
	  canvas->lupeLeft = SCREEN_WIDTH - LUPE_WIDTH/2/4;
      }
      else if(canvas->lupeScale == 3){
	canvas->lupeLeft += LUPE_WIDTH/2/16;
	if(canvas->lupeLeft > SCREEN_WIDTH - LUPE_WIDTH/2/8)
	  canvas->lupeLeft = SCREEN_WIDTH - LUPE_WIDTH/2/8;
      }
      else if(canvas->lupeScale == 4){
	canvas->lupeLeft += LUPE_WIDTH/2/32;
	if(canvas->lupeLeft > SCREEN_WIDTH - LUPE_WIDTH/2/16)
	  canvas->lupeLeft = SCREEN_WIDTH - LUPE_WIDTH/2/16;
      }
      canvas->lupePanel->myReset();
    }
    */
    break;

    //  case 0x1004: // OK
  case Qt::Key_Left:
    /*    if(canvas->zoomStyle == NORMAL){
      canvas->canvasLeft -= SCREEN_WIDTH/2/2;
      if(canvas->canvasLeft < -SCREEN_WIDTH/2)
	canvas->canvasLeft = -SCREEN_WIDTH/2;
    canvas->repaint();
    }
    else if(canvas->zoomStyle == ZOOM){
      if(canvas->zoomScale == 1){
	canvas->zoomLeft -= SCREEN_WIDTH/2/4;
	if(canvas->zoomLeft < -SCREEN_WIDTH/2/2)
	  canvas->zoomLeft = -SCREEN_WIDTH/2/2;
      }
      else if(canvas->zoomScale == 2){
	canvas->zoomLeft -= SCREEN_WIDTH/2/8;
	if(canvas->zoomLeft < -SCREEN_WIDTH/2/4)
	  canvas->zoomLeft = -SCREEN_WIDTH/2/4;
      }
      else if(canvas->zoomScale == 3){
	canvas->zoomLeft -= SCREEN_WIDTH/2/16;
	if(canvas->zoomLeft < -SCREEN_WIDTH/2/8)
	  canvas->zoomLeft = -SCREEN_WIDTH/2/8;
      }
      else if(canvas->zoomScale == 4){
	canvas->zoomLeft -= SCREEN_WIDTH/2/32;
	if(canvas->zoomLeft < -SCREEN_WIDTH/2/16)
	  canvas->zoomLeft = -SCREEN_WIDTH/2/16;
      }
    canvas->repaint();
    }
    else if(canvas->zoomStyle == LUPE){
      if(canvas->lupeScale == 1){
	canvas->lupeLeft -= LUPE_WIDTH/2/4;
	if(canvas->lupeLeft < -LUPE_WIDTH/2/2)
	  canvas->lupeLeft = -LUPE_WIDTH/2/2;
      }
      else if(canvas->lupeScale == 2){
	canvas->lupeLeft -= LUPE_WIDTH/2/8;
	if(canvas->lupeLeft < -LUPE_WIDTH/2/4)
	  canvas->lupeLeft = -LUPE_WIDTH/2/4;
      }
      else if(canvas->lupeScale == 3){
	canvas->lupeLeft -= LUPE_WIDTH/2/16;
	if(canvas->lupeLeft < -LUPE_WIDTH/2/8)
	  canvas->lupeLeft = -LUPE_WIDTH/2/8;
      }
      else if(canvas->lupeScale == 4){
	canvas->lupeLeft -= LUPE_WIDTH/2/32;
	if(canvas->lupeLeft < -LUPE_WIDTH/2/16)
	  canvas->lupeLeft = -LUPE_WIDTH/2/16;
      }
      canvas->lupePanel->myReset();
    }
    */

    break;


  case Qt::Key_X:
    canvas->changeToolbarVisible();
    break;

  case 0x2010://zoom +
    canvas->changeToolbarVisible();
    break;

  default:
    break;
  }

  //  QMainWindow::keyPressEvent(e);
}


void CloverPaint::openImage()
{

  if(canvas->fileChangeFlag){
    if (QMessageBox::warning(this, "CloverPaint",
			     "Changes are not saved.\nOpen anyway?\n",
			     "OK", "Cancel")){
      return;
    }
  }

  SlFileDialog sfd(FALSE, NULL, "Open file(cpf, png, jpg, bmp) ...", TRUE, WType_Modal);

  sfd.move(0,0);

  QDir d(canvas->previousDirectory);
  if(d.exists()){
    sfd.setDefaultFile(canvas->previousDirectory);
  }
  else{
    sfd.setDefaultFile("/home/zaurus/Documents/Image_Files/");
  }


  if(!sfd.exec()){
    return;
  }
 
  if(canvas->zoomStyle == LUPE){
    canvas->dispLupeFlag = false;
    canvas->lupePanel->hide();
  }


  QString filepath = sfd.getFilePath();
  QString filename = sfd.getFileName();


  canvas->previousDirectory = filepath.left(filepath.length() - filename.length());

  if(filename.length() <= 4){
    QMessageBox::warning(this, "CloverPaint", "The file name is invalid.\n","OK");
    return;
  }

  QString ext;
  ext = filepath.right(4);

  if(ext == ".BMP" || ext == ".bmp"   ){

    for(int i=1; i<canvas->layernum; i++){
      delete canvas->layer[i];
    }
    canvas->layernum = 1;
    canvas->currentlayer = 0;
    canvas->layerPanel->RebuildPanel();


    QImage *tmpImage = new QImage();
    if(!tmpImage->load(filepath,"BMP")){
	if (!QMessageBox::warning(this, "CloverPaint", "Load error.\n","OK")){
	  delete tmpImage;
	  return;
	}
    }
    int tmpWidth = 0;
    int tmpHeight = 0;
    if(tmpImage->width()< 480)
      tmpWidth = tmpImage->width();
    else
      tmpWidth = 480;
    if(tmpImage->height()<640)
      tmpHeight = tmpImage->height();
    else
      tmpHeight = 640;
    
    if(tmpWidth != 480 || tmpHeight !=640){
      for(int y=0; y<640; y++){
	for(int x=0; x<480; x++){
	  canvas->layer[0]->setPixel(x,y,0xffffffff);
	}
      }
    }
    for(int y=0; y<tmpHeight; y++){
      for(int x=0; x<tmpWidth; x++){
	canvas->layer[0]->setPixel(x,y, tmpImage->pixel(x,y));
      }
    }
    delete tmpImage;

    canvas->layerMode[0] = NORMAL;
    canvas->layerVisible[0] = true;
    canvas->layerDrawable[0] = true;
    canvas->layerAlpha[0] = 100;
    canvas->layerAlpha2[0] = 255;
    canvas->layerNameCounter = 1;


    canvas->repaint();
    canvas->layerPanel->RebuildPanel();
    canvas->toolbar->redrawToolBar();

  }
  else if(ext == ".JPG" || ext == ".jpg"  ){
    for(int i=1; i<canvas->layernum; i++){
      delete canvas->layer[i];
    }
    canvas->layernum = 1;
    canvas->currentlayer = 0;
    canvas->layerPanel->RebuildPanel();

    QImage *tmpImage = new QImage();

    if(!tmpImage->load(filepath,"JPEG")){
	if (!QMessageBox::warning(this, "CloverPaint", "Load error.\n","OK")){

	  delete tmpImage;

	  return;
	}
    }
    int tmpWidth = 0;
    int tmpHeight = 0;
    if(tmpImage->width()< 480)
      tmpWidth = tmpImage->width();
    else
      tmpWidth = 480;
    if(tmpImage->height()<640)
      tmpHeight = tmpImage->height();
    else
      tmpHeight = 640;
    
    if(tmpWidth != 480 || tmpHeight !=640){
      for(int y=0; y<640; y++){
	for(int x=0; x<480; x++){
	  canvas->layer[0]->setPixel(x,y,0xffffffff);
	}
      }
    }
    for(int y=0; y<tmpHeight; y++){
      for(int x=0; x<tmpWidth; x++){
	canvas->layer[0]->setPixel(x,y, tmpImage->pixel(x,y));
      }
    }
    delete tmpImage;

    canvas->layerMode[0] = NORMAL;
    canvas->layerVisible[0] = true;
    canvas->layerDrawable[0] = true;
    canvas->layerAlpha[0] = 100;
    canvas->layerAlpha2[0] = 255;
    canvas->layerNameCounter = 1;

    canvas->repaint();
    canvas->layerPanel->RebuildPanel();
    canvas->toolbar->redrawToolBar();

  }
  else if(ext == ".PNG"|| ext == ".png" ){
    for(int i=1; i<canvas->layernum; i++){
      delete canvas->layer[i];
    }
    canvas->layernum = 1;
    canvas->currentlayer = 0;
    canvas->layerPanel->RebuildPanel();

    QImage *tmpImage = new QImage();

    if(!tmpImage->load(filepath,"PNG")){
	if (!QMessageBox::warning(this, "CloverPaint", "Load error.\n","OK")){
	  delete tmpImage;
	  return;
	}
    }
    int tmpWidth = 0;
    int tmpHeight = 0;
    if(tmpImage->width()< 480)
      tmpWidth = tmpImage->width();
    else
      tmpWidth = 480;
    if(tmpImage->height()<640)
      tmpHeight = tmpImage->height();
    else
      tmpHeight = 640;
    
    if(tmpWidth != 480 || tmpHeight !=640){
      for(int y=0; y<640; y++){
	for(int x=0; x<480; x++){
	  canvas->layer[0]->setPixel(x,y,0xffffffff);
	}
      }
    }
    for(int y=0; y<tmpHeight; y++){
      for(int x=0; x<tmpWidth; x++){
	canvas->layer[0]->setPixel(x,y, tmpImage->pixel(x,y));
      }
    }
    delete tmpImage;

    canvas->layerMode[0] = NORMAL;
    canvas->layerVisible[0] = true;
    canvas->layerDrawable[0] = true;
    canvas->layerAlpha[0] = 100;
    canvas->layerAlpha2[0] = 255;
    canvas->layerNameCounter = 1;

    canvas->repaint();
    canvas->layerPanel->RebuildPanel();
    canvas->toolbar->redrawToolBar();

  }
  else if(ext == ".cpf" || ext == ".CPF"){
    for(int i=0; i<canvas->layernum; i++){
      delete canvas->layer[i];
    }

    loadCpfImage(filepath);

    canvas->repaint();
    canvas->layerPanel->RebuildPanel();
    canvas->toolbar->redrawToolBar();

  }
  else{
    if (QMessageBox::warning(this, "CloverPaint",
			     "Unsupported image format.\n(CloverPaint supports .cpf .png .jpg .bmp)",
			     "OK")){
      return;
    }

  }

  canvas->currentFilename = filename;
  canvas->fileChangeFlag = false;


}




void CloverPaint::loadLayer()
{

  SlFileDialog sfd(FALSE, NULL, "Load layer(png, jpg, bmp) ...", TRUE, WType_Modal);

  sfd.move(0,0);

  QDir d(canvas->previousDirectory);
  if(d.exists()){
    sfd.setDefaultFile(canvas->previousDirectory);
  }
  else{
    sfd.setDefaultFile("/home/zaurus/Documents/Image_Files/");
  }


  if(!sfd.exec()){
    return;
  }
  
  QString filepath = sfd.getFilePath();
  QString filename = sfd.getFileName();


  canvas->previousDirectory = filepath.left(filepath.length() - filename.length());


  if(filename.length() <= 4){
    QMessageBox::warning(this, "CloverPaint", "The file name is invalid.\n","OK");
    return;
  }

  QString ext;
  ext = filepath.right(4);



  if(ext == ".BMP" || ext == ".bmp"   ){


    QImage *tmpImage = new QImage();
    if(!tmpImage->load(filepath,"BMP")){
	if (!QMessageBox::warning(this, "CloverPaint", "Load error.\n","OK")){
	  delete tmpImage;
	  return;
	}
    }
    int tmpWidth = 0;
    int tmpHeight = 0;
    if(tmpImage->width()< 480)
      tmpWidth = tmpImage->width();
    else
      tmpWidth = 480;
    if(tmpImage->height()<640)
      tmpHeight = tmpImage->height();
    else
      tmpHeight = 640;
    
    if(tmpWidth != 480 || tmpHeight !=640){
      for(int y=0; y<640; y++){
	for(int x=0; x<480; x++){
	  canvas->layer[canvas->currentlayer]->setPixel(x,y,0xffffffff);
	}
      }
    }
    for(int y=0; y<tmpHeight; y++){
      for(int x=0; x<tmpWidth; x++){
	canvas->layer[canvas->currentlayer]->setPixel(x,y, tmpImage->pixel(x,y));
      }
    }
    delete tmpImage;

    canvas->repaint();
    canvas->toolbar->redrawToolBar();

  }
  else if(ext == ".JPG" || ext == ".jpg"  ){


    QImage *tmpImage = new QImage();
    if(!tmpImage->load(filepath,"JPEG")){
	if (!QMessageBox::warning(this, "CloverPaint", "Load error.\n","OK")){
	  delete tmpImage;
	  return;
	}
    }
    int tmpWidth = 0;
    int tmpHeight = 0;
    if(tmpImage->width()< 480)
      tmpWidth = tmpImage->width();
    else
      tmpWidth = 480;
    if(tmpImage->height()<640)
      tmpHeight = tmpImage->height();
    else
      tmpHeight = 640;
    
    if(tmpWidth != 480 || tmpHeight !=640){
      for(int y=0; y<640; y++){
	for(int x=0; x<480; x++){
	  canvas->layer[canvas->currentlayer]->setPixel(x,y,0xffffffff);
	}
      }
    }
    for(int y=0; y<tmpHeight; y++){
      for(int x=0; x<tmpWidth; x++){
	canvas->layer[canvas->currentlayer]->setPixel(x,y, tmpImage->pixel(x,y));
      }
    }
    delete tmpImage;

    canvas->repaint();
    canvas->toolbar->redrawToolBar();

  }
  else if(ext == ".PNG"|| ext == ".png" ){


    QImage *tmpImage = new QImage();
    if(!tmpImage->load(filepath,"PNG")){
	if (!QMessageBox::warning(this, "CloverPaint", "Load error.\n","OK")){
	  delete tmpImage;
	  return;
	}
    }

    int tmpWidth = 0;
    int tmpHeight = 0;
    if(tmpImage->width()< 480)
      tmpWidth = tmpImage->width();
    else
      tmpWidth = 480;
    if(tmpImage->height()<640)
      tmpHeight = tmpImage->height();
    else
      tmpHeight = 640;


    if(tmpWidth != 480 || tmpHeight !=640){
      for(int y=0; y<640; y++){
	for(int x=0; x<480; x++){
	  canvas->layer[canvas->currentlayer]->setPixel(x,y,0xffffffff);
	}
      }
    }


    for(int y=0; y<tmpHeight; y++){
      for(int x=0; x<tmpWidth; x++){
	canvas->layer[canvas->currentlayer]->setPixel(x,y, tmpImage->pixel(x,y));
      }
    }
    delete tmpImage;


    canvas->repaint();
    canvas->toolbar->redrawToolBar();


  }
  else{
    if (QMessageBox::warning(this, "CloverPaint",
			     "Unsupported image format.\n(CloverPaint supports .png .jpg .bmp)",
			     "OK")){
      return;
    }

  }

  canvas->fileChangeFlag = true;


}



void CloverPaint::resize(){
  setFixedSize(QApplication::desktop()->size());
  showNormal();
  reparent( NULL, WType_TopLevel | WStyle_Customize | WStyle_NoBorderEx, QPoint(0, 0));

  showFullScreen();
}




void CloverPaint::saveLayer()
{

  SlFileDialog sfd(TRUE, NULL, "Save Layer (PNG) ...", TRUE, WType_Modal);
  sfd.move(0,0);

  QDir d(canvas->previousDirectory);
  if(d.exists()){
    sfd.setDefaultFile(canvas->previousDirectory);
  }
  else{
    sfd.setDefaultFile("/home/zaurus/Documents/Image_Files/");
  }

  Global::showInputMethod();

  QString filename2 = "";
  if(canvas->currentFilename !=  ""){
    //入力前のファイル名を保存
    filename2 = canvas->currentFilename;
    canvas->currentFilename =  canvas->currentFilename.left(canvas->currentFilename.length()-4) + ".png";
    sfd.setDefaultName(canvas->currentFilename);
  }
  else{
    //入力前のファイル名を保存

    QDate date = QDate::currentDate();
    QTime time = QTime::currentTime();
    QString tmpDateStr = QString("20000101120000");
    tmpDateStr = tmpDateStr.sprintf("%d%.2d%.2d_%.2d%.2d_%.2d", date.year(), date.month(),date.day(), time.hour(),time.minute(),time.second());
    canvas->currentFilename = tmpDateStr + ".png";
    sfd.setDefaultName(canvas->currentFilename);


  }

  if(!sfd.exec()){
    Global::hideInputMethod();
    return;
  }

  Global::hideInputMethod();

  QString filepath = sfd.getFilePath();
  QString tmpFilepath = sfd.getFilePath();
  QString filename = sfd.getFileName();

  // ディレクトリ名を取得
  canvas->previousDirectory = filepath.left(filepath.length() - filename.length());

  if(filename.find(".png", 0, FALSE) == -1){
    filename += ".png";
  }

  // ファイルパスを決定
  filepath = canvas->previousDirectory + filename;

  if(filepath != tmpFilepath && QFile::exists(filepath)){
    if (QMessageBox::warning(this, "CloverPaint",
			     "The same file name exists.\nSave anyway?\n",
			     "OK", "Cancel")){
      canvas->currentFilename = filename2;

      return;
    }
  }

  // ピクセル透明度の白マット化
    for(int y=0; y<640; y++){
      for(int x=0; x<480; x++){
	canvas->bgImage->setPixel(x,y,
	   canvas->calc(NORMAL, 0xffffffff, canvas->layer[canvas->currentlayer]->pixel(x,y), 256));
      }
    }

  if(canvas->canvasType == VGA){ 
    if(!canvas->bgImage->save(filepath,"PNG")){
      if (!QMessageBox::warning(this, "CloverPaint", "Save error.\n","OK")){
	return;
      }
    }
  }
  else if(canvas->canvasType == QVGA){ 
    QImage qvgaLayer = canvas->bgImage->copy(0,0,240,320,0);
    if(!qvgaLayer.save(filepath,"PNG")){
      if (!QMessageBox::warning(this, "CloverPaint", "Save error.\n","OK")){
	return;
      }
    }
  }

  //  canvas->currentFilename = filename;

}


void CloverPaint::saveImage()
{
  QString ext;
  if(canvas->currentFilename.length() > 4){
    ext = canvas->currentFilename.right(4);
    if(ext != ".cpf"){
      saveCpfImage();
      return;
    }
  }
  else{
      saveCpfImage();
      return;
  }

  canvas->currentFilename = canvas->currentFilename.left(canvas->currentFilename.length()-4) + ".cpf";

  QString filepath = canvas->previousDirectory + canvas->currentFilename;


  //Save as CPF(CloverPaintFormat) image ...
  
  QFile f(filepath);
  f.open( IO_WriteOnly );
  QDataStream s( &f );


  //CPFヘッダ
  char cloverHeader[] = "CPF"; //CPF形式 :3byte
  char versionHeader[] = "1.0"; //バージョン :3byte
  unsigned char metaHeaderLength = 4; //以下に続くその他メタ情報のバイト数．255まで指定可能．:1byte
  char layernumHeader = canvas->layernum; //メタ情報(レイヤ数) :1byte
  short layerNameCounterHeader = canvas->layerNameCounter; //メタ情報(レイヤ名のカウンタ) :2byte
  char canvasTypeHeader = canvas->canvasType; // 1byte

  s.writeRawBytes((const char*)cloverHeader, 3);
  s.writeRawBytes((const char*)versionHeader, 3);
  s.writeRawBytes((const char*)&metaHeaderLength, 1);
  s.writeRawBytes((const char*)&layernumHeader, 1);
  s.writeRawBytes((const char*)&layerNameCounterHeader, 2);
  s.writeRawBytes((const char*)&canvasTypeHeader, 1);

  //レイヤ
  QImageIO iio;
  iio.setIODevice(&f);
  iio.setFormat("PNG");

  char layerVisibleInfo = 0;
  unsigned char layerMetaHeaderLength = 0; //以下に続くレイヤのその他メタ情報のバイト数．255まで指定可能．:1byte

  for(int i=0; i<canvas->layernum; i++){
    s.writeRawBytes((const char*)&layerMetaHeaderLength, 1);
    s.writeRawBytes((const char*)canvas->layerName[i], 11); //レイヤ名 : 11byte

    if(canvas->layerVisible[i]){
      layerVisibleInfo = 1;
    }
    else{
      layerVisibleInfo = 0;
    }
    s.writeRawBytes((const char*)&layerVisibleInfo, 1); //レイヤの可視性 : 1byte
    s.writeRawBytes((const char*)&(canvas->layerMode[i]), 1); //レイヤモード : 1byte
    s.writeRawBytes((const char*)&(canvas->layerAlpha[i]), 1); //レイヤアルファ : 1byte
    canvas->layerAlpha2[i] = (int)((double)(canvas->layerAlpha[i]*2.55));
    if(canvas->layerAlpha2[i]>255){
	canvas->layerAlpha2[i] = 255;
      }

    if(canvas->canvasType == VGA){
      iio.setImage( *(canvas->layer[i]) ); //レイヤをPNGとして描き込み
    }
    else if(canvas->canvasType == QVGA){ 
      QImage qvgaLayer = canvas->layer[i]->copy(0,0,240,320,0);
      iio.setImage(qvgaLayer);
    }

    if(!iio.write()){
	if (!QMessageBox::warning(this, "CloverPaint", "Save error.\n","OK")){
	  f.close();
	  f.remove();

	  return;
	}
    }
    // 透明度を保存してくれないので、別のPNGファイルとして保存
    for(int y=0; y<640; y++){
      for(int x=0; x<480; x++){
	canvas->bgImage->setPixel(x,y,(canvas->layer[i]->pixel(x,y)>>24) & 0x000000ff);
      }
    }
    
    if(canvas->canvasType == VGA){ 
      iio.setImage( *(canvas->bgImage) ); //透明度をPNGとして描き込み
    }
    else if(canvas->canvasType == QVGA){ 
      QImage qvgaLayer = canvas->bgImage->copy(0,0,240,320,0);
      iio.setImage(qvgaLayer);
    }

    if(!iio.write()){
	if (!QMessageBox::warning(this, "CloverPaint", "Save error.\n","OK")){
	  canvas->currentFilename = "";
	  f.close();
	  f.remove();

	  return;
	}
    }

  }

  f.close();
  
  canvas->fileChangeFlag = false;

}


void CloverPaint::saveJpgImage()
{

  SlFileDialog sfd(TRUE, NULL, "Save JPG ...", TRUE, WType_Modal);
  sfd.move(0,0);

  QDir d(canvas->previousDirectory);
  if(d.exists()){
    sfd.setDefaultFile(canvas->previousDirectory);
  }
  else{
    sfd.setDefaultFile("/home/zaurus/Documents/Image_Files/");
  }


  Global::showInputMethod();

  QString filename2 = "";
  if(canvas->currentFilename !=  ""){
    //入力前のファイル名を保存
    filename2 = canvas->currentFilename;
    canvas->currentFilename =  canvas->currentFilename.left(canvas->currentFilename.length()-4) + ".jpg";
    sfd.setDefaultName(canvas->currentFilename);
  }
  else{
    //入力前のファイル名を保存

    QDate date = QDate::currentDate();
    QTime time = QTime::currentTime();
    QString tmpDateStr = QString("20000101120000");
    tmpDateStr = tmpDateStr.sprintf("%d%.2d%.2d_%.2d%.2d_%.2d", date.year(), date.month(),date.day(), time.hour(),time.minute(),time.second());
    canvas->currentFilename = tmpDateStr + ".jpg";
    sfd.setDefaultName(canvas->currentFilename);

  }

  if(!sfd.exec()){
    Global::hideInputMethod();
    canvas->currentFilename = filename2;
    return;
  }

  Global::hideInputMethod();

  QString filepath = sfd.getFilePath();
  QString tmpFilepath = sfd.getFilePath();
  QString filename = sfd.getFileName();


  // ディレクトリ名を取得
  canvas->previousDirectory = filepath.left(filepath.length() - filename.length());

  if(filename.find(".jpg", 0, FALSE) == -1){
    filename += ".jpg";
  }


  // ファイルパスを決定
  filepath = canvas->previousDirectory + filename;

  if(filepath != tmpFilepath && QFile::exists(filepath)){
    if (QMessageBox::warning(this, "CloverPaint",
			     "The same file name exists.\nSave anyway?\n",
			     "OK", "Cancel")){
      canvas->currentFilename = filename2;

      return;
    }
  }


  if(canvas->canvasType == VGA){ 
    if(!canvas->bgImage->save(filepath,"JPEG")){
      if (!QMessageBox::warning(this, "CloverPaint", "Save error.\n","OK")){
	return;
      }
    }
  }
  else if(canvas->canvasType == QVGA){ 
    QImage qvgaLayer = canvas->bgImage->copy(0,0,240,320,0);
    if(!qvgaLayer.save(filepath,"JPEG")){
      if (!QMessageBox::warning(this, "CloverPaint", "Save error.\n","OK")){
	return;
      }
    }
  }



  canvas->fileChangeFlag = false;

  canvas->currentFilename = filename;


}


void CloverPaint::savePngImage()
{

  SlFileDialog sfd(TRUE, NULL, "Save PNG ...", TRUE, WType_Modal);
  sfd.move(0,0);

  QDir d(canvas->previousDirectory);
  if(d.exists()){
    sfd.setDefaultFile(canvas->previousDirectory);
  }
  else{
    sfd.setDefaultFile("/home/zaurus/Documents/Image_Files/");
  }

  Global::showInputMethod();

  QString filename2 = "";
  if(canvas->currentFilename !=  ""){
    //入力前のファイル名を保存
    filename2 = canvas->currentFilename;
    canvas->currentFilename =  canvas->currentFilename.left(canvas->currentFilename.length()-4) + ".png";
    sfd.setDefaultName(canvas->currentFilename);
  }
  else{
    //入力前のファイル名を保存

    QDate date = QDate::currentDate();
    QTime time = QTime::currentTime();
    QString tmpDateStr = QString("20000101120000");
    tmpDateStr = tmpDateStr.sprintf("%d%.2d%.2d_%.2d%.2d_%.2d", date.year(), date.month(),date.day(), time.hour(),time.minute(),time.second());
    canvas->currentFilename = tmpDateStr + ".png";
    sfd.setDefaultName(canvas->currentFilename);


  }


  if(!sfd.exec()){
    Global::hideInputMethod();
    canvas->currentFilename = filename2;
    return;
  }

  Global::hideInputMethod();

  QString filepath = sfd.getFilePath();
  QString tmpFilepath = sfd.getFilePath();
  QString filename = sfd.getFileName();


  // ディレクトリ名を取得
  canvas->previousDirectory = filepath.left(filepath.length() - filename.length());

  if(filename.find(".png", 0, FALSE) == -1){
    filename += ".png";
  }


  // ファイルパスを決定
  filepath = canvas->previousDirectory + filename;

  if(filepath != tmpFilepath && QFile::exists(filepath)){
    if (QMessageBox::warning(this, "CloverPaint",
			     "The same file name exists.\nSave anyway?\n",
			     "OK", "Cancel")){
      canvas->currentFilename = filename2;

      return;
    }
  }

  if(canvas->canvasType == VGA){ 
    if(!canvas->bgImage->save(filepath,"PNG")){
      if (!QMessageBox::warning(this, "CloverPaint", "Save error.\n","OK")){
	return;
      }
    }
  }
  else if(canvas->canvasType == QVGA){ 
    QImage qvgaLayer = canvas->bgImage->copy(0,0,240,320,0);
    if(!qvgaLayer.save(filepath,"PNG")){
      if (!QMessageBox::warning(this, "CloverPaint", "Save error.\n","OK")){
	return;
      }
    }
  }


  canvas->fileChangeFlag = false;

  canvas->currentFilename = filename;

}


void CloverPaint::saveCpfImage()
{

  SlFileDialog sfd(TRUE, NULL, "Save CPF ...", TRUE, WType_Modal);
  sfd.move(0,0);

  QDir d(canvas->previousDirectory);
  if(d.exists()){
    sfd.setDefaultFile(canvas->previousDirectory);
  }
  else{
    sfd.setDefaultFile("/home/zaurus/Documents/Image_Files/");
  }

  Global::showInputMethod();

  QString filename2 = "";
  if(!canvas->currentFilename.isEmpty()){
    //入力前のファイル名を保存
    filename2 = canvas->currentFilename;
    canvas->currentFilename = canvas->currentFilename.left(canvas->currentFilename.length() -4) + ".cpf";
    sfd.setDefaultName(canvas->currentFilename);
  }
  else{
    //入力前のファイル名を保存
  
    QDate date = QDate::currentDate();
    QTime time = QTime::currentTime();
    QString tmpDateStr = QString("20000101120000");
    tmpDateStr = tmpDateStr.sprintf("%d%.2d%.2d_%.2d%.2d_%.2d", date.year(), date.month(),date.day(), time.hour(),time.minute(),time.second());
    canvas->currentFilename = QString(tmpDateStr) + ".cpf";
    sfd.setDefaultName(canvas->currentFilename);
  }

  if(!sfd.exec()){
    Global::hideInputMethod();
    canvas->currentFilename = filename2;    

    return;
  }

  Global::hideInputMethod();
  
  QString filepath = sfd.getFilePath();
  QString tmpFilepath = sfd.getFilePath();
  QString filename = sfd.getFileName();

  // ディレクトリ名を取得
  canvas->previousDirectory = filepath.left(filepath.length() - filename.length());

  if(filename.find(".cpf", 0, FALSE) == -1){
    filename += ".cpf";
  }

  // ファイルパスを決定
  filepath = canvas->previousDirectory + filename;

  // システムによるファイル名変更後，
  // もう一度重複チェック
  if(filepath != tmpFilepath && QFile::exists(filepath)){
    if (QMessageBox::warning(this, "CloverPaint",
			     "The same file name exists.\nSave anyway?\n",
			     "OK", "Cancel")){
      canvas->currentFilename = filename2;

      
      return;
    }
  }

  //Save as CPF(CloverPaintFormat) image ...
  
  QFile f(filepath);
  f.open( IO_WriteOnly );
  QDataStream s( &f );


  //CPFヘッダ
  char cloverHeader[] = "CPF"; //CPF形式 :3byte
  char versionHeader[] = "1.0"; //バージョン :3byte
  unsigned char metaHeaderLength = 4; //以下に続くその他メタ情報のバイト数．255まで指定可能．:1byte
  char layernumHeader = canvas->layernum; //メタ情報(レイヤ数) :1byte
  short layerNameCounterHeader = canvas->layerNameCounter; //メタ情報(レイヤ名のカウンタ) :2byte
  char canvasTypeHeader = canvas->canvasType; // 1byte

  s.writeRawBytes((const char*)cloverHeader, 3);
  s.writeRawBytes((const char*)versionHeader, 3);
  s.writeRawBytes((const char*)&metaHeaderLength, 1);
  s.writeRawBytes((const char*)&layernumHeader, 1);
  s.writeRawBytes((const char*)&layerNameCounterHeader, 2);
  s.writeRawBytes((const char*)&canvasTypeHeader, 1);

  //レイヤ
  QImageIO iio;
  iio.setIODevice(&f);
  iio.setFormat("PNG");

  char layerVisibleInfo = 0;
  unsigned char layerMetaHeaderLength = 0; //以下に続くレイヤのその他メタ情報のバイト数．255まで指定可能．:1byte

  for(int i=0; i<canvas->layernum; i++){
    s.writeRawBytes((const char*)&layerMetaHeaderLength, 1);
    s.writeRawBytes((const char*)canvas->layerName[i], 11); //レイヤ名 : 11byte
    if(canvas->layerVisible[i]){
      layerVisibleInfo = 1;
    }
    else{
      layerVisibleInfo = 0;
    }
    s.writeRawBytes((const char*)&layerVisibleInfo, 1); //レイヤの可視性 : 1byte
    s.writeRawBytes((const char*)&(canvas->layerMode[i]), 1); //レイヤモード : 1byte
    s.writeRawBytes((const char*)&(canvas->layerAlpha[i]), 1); //レイヤアルファ : 1byte
    canvas->layerAlpha2[i] = (int)((double)(canvas->layerAlpha[i]*2.55));
    if(canvas->layerAlpha2[i]>255){
	canvas->layerAlpha2[i] = 255;
      }

    if(canvas->canvasType == VGA){ 
      iio.setImage( *(canvas->layer[i]) ); //レイヤをPNGとして描き込み
    }
    else if(canvas->canvasType == QVGA){ 
      QImage qvgaLayer = canvas->layer[i]->copy(0,0,240,320,0);
      iio.setImage(qvgaLayer);
    }

    if(!iio.write()){
	if (!QMessageBox::warning(this, "CloverPaint", "Save error.\n","OK")){
	  canvas->currentFilename = "";
	  f.close();
	  f.remove();

	  return;
	}
    }
    // 透明度を保存してくれないので、別のPNGファイルとして保存
    for(int y=0; y<640; y++){
      for(int x=0; x<480; x++){
	canvas->bgImage->setPixel(x,y,(canvas->layer[i]->pixel(x,y)>>24) & 0x000000ff);
      }
    }
    
    if(canvas->canvasType == VGA){ 
      iio.setImage( *(canvas->bgImage) ); //透明度をPNGとして描き込み
    }
    else if(canvas->canvasType == QVGA){ 
      QImage qvgaLayer = canvas->bgImage->copy(0,0,240,320,0);
      iio.setImage(qvgaLayer);
    }

    if(!iio.write()){
	if (!QMessageBox::warning(this, "CloverPaint", "Save error.\n","OK")){
	  canvas->currentFilename = "";
	  f.close();
	  f.remove();

	  return;
	}
    }

  }

  f.close();

  canvas->fileChangeFlag = false;

  canvas->currentFilename = filename;


}


void CloverPaint::loadCpfImage(QString filename)
{

  QFile f(filename);
  f.open( IO_ReadOnly );
  QDataStream s( &f );

  //  fprintf(stderr, "header loading ...\n");

  //CPFヘッダ
  char cloverHeader[] = "CPF"; //CPF形式 :3byte
  char versionHeader[] = "1.0"; //バージョン :3byte
  unsigned char metaHeaderLength = 3; //以下に続くその他メタ情報のバイト数．255まで指定可能．:1byte

  s.readRawBytes((char*)cloverHeader, 3);
  s.readRawBytes((char*)versionHeader, 3);
  if(strcmp(cloverHeader, "CPF") != 0){
    fprintf(stderr, "invalid CPF header.\n");    
    return;
  }
  if(strcmp(versionHeader, "1.0") != 0){
    fprintf(stderr, "unknown CPF version.\n");    
    return;
  }
  s.readRawBytes((char*)&metaHeaderLength, 1);
  if(metaHeaderLength == 3){
    s.readRawBytes((char*)&(canvas->layernum), 1);
    s.readRawBytes((char*)&(canvas->layerNameCounter), 2);
    canvas->canvasType = VGA;
  }
  else if(metaHeaderLength == 4){
    s.readRawBytes((char*)&(canvas->layernum), 1);
    s.readRawBytes((char*)&(canvas->layerNameCounter), 2);
    s.readRawBytes((char*)&(canvas->canvasType), 1);
  }

  if(canvas->canvasType == VGA){
    canvas->zoomScale = 0; //シフト数で指定。 0 は等倍．
    canvas->lastScale = 1; //最後に選択された等倍以上の拡大率
    canvas->lupeScale = 2; //シフト数で指定。 0 は等倍．
    canvas->zoomStyle = NORMAL;
    canvas->zoomTop = 120;
    canvas->zoomLeft = 160;
    canvas->lupeTop = 210 ;
    canvas->lupeLeft = 130;

  }
  else{
    canvas->zoomScale = 1;
    canvas->lupeScale = 2;
    canvas->zoomStyle = ZOOM;
    canvas->lastScale = 2; //最後に選択された等倍以上の拡大率
    canvas->zoomTop = 0;
    canvas->zoomLeft = 0;
    canvas->lupeTop = 105+22;
    canvas->lupeLeft = 65+22;
  }

  //  fprintf(stderr, "header is loaded.\n");

  //レイヤ
  QImageIO iio;
  iio.setIODevice(&f);
  iio.setFormat("PNG");

  char layerVisibleInfo = 0;
  unsigned char layerMetaHeaderLength = 14; //以下に続くレイヤのその他メタ情報のバイト数．255まで指定可能．:1byte

  for(int i=0; i<canvas->layernum; i++){
    //    fprintf(stderr, "layer %d loading ...\n",i);

    canvas->layer[i] = new QImage();
    if(!canvas->layer[i]->create(SCREEN_WIDTH, SCREEN_HEIGHT, 32,0,QImage::IgnoreEndian)){
      if (!QMessageBox::warning(this, "CloverPaint", "Sorry, no available memory...\n","OK")){
	canvas->clover->myResize();
	
	return;
      }
    }
    
    char tmpLayerName[] = "           ";
    s.readRawBytes((char*)&layerMetaHeaderLength, 1);
    s.readRawBytes((char*)tmpLayerName, 11); //レイヤ名 : 11byte
    s.readRawBytes((char*)&layerVisibleInfo, 1); //レイヤの可視性 : 1byte
    canvas->layerName[i] = tmpLayerName;

    if(layerVisibleInfo == 1){
      canvas->layerVisible[i]=true;
    }
    else{
      canvas->layerVisible[i]=false;
    }
    canvas->layerDrawable[i]=true;

    s.readRawBytes((char*)&(canvas->layerMode[i]), 1); //レイヤモード : 1byte
    s.readRawBytes((char*)&(canvas->layerAlpha[i]), 1); //レイヤアルファ : 1byte

    int tmpAlpha2 = (int)((double)canvas->layerAlpha[i]*2.56);
    if(tmpAlpha2 > 256){
	tmpAlpha2 = 256;
    }
    canvas->layerAlpha2[i] = tmpAlpha2;

    QImage tmpImage1;
    QImage tmpImage2;
    //レイヤ
    if(iio.read()){
      tmpImage1 = (iio.image()).copy();
    }
    else{
      QMessageBox::warning(this, "CloverPaint", "failed to read PNGed layer image.","OK");
      return;
    }
    //透明度
    if(iio.read()){
      tmpImage2 = (iio.image()).copy();
    }
    else{
      QMessageBox::warning(this, "CloverPaint", "failed to read PNGed layer image.","OK");

      return;
    }

    if(canvas->canvasType == VGA){
      for(int y=0; y<640; y++){
	for(int x=0; x<480; x++){
	  canvas->layer[i]->setPixel(x,y, (tmpImage1.pixel(x,y) & 0x00ffffff) | ((tmpImage2.pixel(x,y)<<24) & 0xff000000));
	}
      }
    }
    else if(canvas->canvasType == QVGA){
      for(int y=0; y<320; y++){
	for(int x=0; x<240; x++){
	  canvas->layer[i]->setPixel(x,y, (tmpImage1.pixel(x,y) & 0x00ffffff) | ((tmpImage2.pixel(x,y)<<24) & 0xff000000));
	}
      }
    }

  }

  f.close();

  canvas->currentlayer = 0;

  resize();

 
}


void CloverPaint::myResize()
{
  resize();
}

void CloverPaint::closeEvent( QCloseEvent *e )
{
  if(canvas->fileChangeFlag){
    if (QMessageBox::warning(this, "CloverPaint",
			     "Changes are not saved.\nExit anyway?\n",
			     "OK", "Cancel")){
      e->ignore();
    }
    else{
      //スクリーンセーバー再開
      //      QCopEnvelope setScreenSaverMode( "QPE/System", "setScreenSaverMode(int)" );
      //      setScreenSaverMode << 3;

      e->accept();
    }
  }
  else{
    //    QCopEnvelope setScreenSaverMode( "QPE/System", "setScreenSaverMode(int)" );
    //    setScreenSaverMode << 3;

    e->accept();
  }
}

