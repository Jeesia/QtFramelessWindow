QtFramelessWindow
=================

Qt4 on Windows

Example:

    class MyWindow : public FramelessWindow<QWidget>
    {
        ...
    };
    
    int main(int argc, char *argv[])
    {
    	QApplication a(argc, argv);
    	MyWindow w;
    	w.show();
    	return a.exec();
    }
