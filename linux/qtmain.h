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

    void mouseMoveEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent *event) override;

    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

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
