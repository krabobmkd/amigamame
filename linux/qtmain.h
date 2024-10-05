#include <QLabel>
#include <QTimer>
class QThread;

class QWin : public QLabel
{
    Q_OBJECT
public:
    QWin();
public slots:
    void updateWin();
signals:
    void startproc();
protected:
	QTimer		m_timer;
};

class QProc : public QObject
{
    Q_OBJECT
public:
    QProc();
public slots:
    void process();
signals:
    void updateWin();

};
