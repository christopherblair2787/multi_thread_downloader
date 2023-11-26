#include <QWidget>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QAbstractItemView>
#include <QDateTime>
#include <QMessageBox>
#include <QThread>
#include <QDialog>
#include <QDebug>
#include "ui_about.h"
#include "mainwindow.h"
#include "newtaskdialog.h"
#include "deletetaskdialog.h"
#include "modifytaskdialog.h"
#include "preferencesdialog.h"
#include "tools.h"

#ifdef WIN32
#include <windows.h>
#endif

MainWindow::MainWindow( QWidget * parent, Qt::WFlags f)
		: QMainWindow(parent, f)
{
	QString language = tr("English");	//Translate English into your own language
	createTaskTableWidget();
	createThreadNumberTreeWidget();
	createTaskGraphPainterWidget();
	createThreadsDetailTableWidget();

	taskGraphDetailLayout = new QHBoxLayout();
	taskGraphDetailLayout->addWidget(threadsDetailTableWidget);
	taskGraphDetailLayout->addWidget(taskGraphWidget);

	threadGraphDetailWidget = new QWidget();
	threadGraphDetailWidget->setLayout(taskGraphDetailLayout);

	infoDetailSplitter = new QSplitter(Qt::Horizontal);
	infoDetailSplitter->addWidget(threadNumberTreeWidget);
	infoDetailSplitter->addWidget(threadGraphDetailWidget);
	QList <int> infoDetailSplitterList;
	infoDetailSplitterList<<130<<600;
	infoDetailSplitter->setSizes(infoDetailSplitterList);
	infoDetailSplitter->setStretchFactor(1, 1);

	taskInfoSplitter = new QSplitter(Qt::Vertical);
	taskInfoSplitter->addWidget(tableWidget);
	taskInfoSplitter->addWidget(infoDetailSplitter);
	QList <int> taskInfoSplitterList;
	taskInfoSplitterList<<300<<300;
	taskInfoSplitter->setSizes(taskInfoSplitterList);

	setCentralWidget(taskInfoSplitter);

	createActions();
	createTrayIcon();
	createMenus();
	createToolBars();
	createSpeedWidgetDock();

	createTaskInfomation();

	createTaskTableWidgetContextMenu();
	createTaskThreadsDetailTableWidgetContextMenu();

	createSmallWindow();

	resize(800, 600);
	setWindowTitle(tr("NetFleet"));
	QIcon icon = QIcon(":/images/icon.png");
	setWindowIcon(icon);

	deleteTaskTimer = 0;
	quitApplicationTimer = 0;

	readSettings();

	clipboard = QApplication::clipboard();
	connect(clipboard, SIGNAL(dataChanged()), this, SLOT(acceptClipboard()));
}
void MainWindow::createActions()
{
	quitAct = new QAction(QIcon(":/images/exit.png"),tr("&Quit"), this);
	quitAct->setShortcut(tr("Ctrl+Q"));
	quitAct->setStatusTip(tr("Quit"));
	connect(quitAct, SIGNAL(triggered()), this, SLOT(quitTriggered()));

	addTaskAct = new QAction(QIcon(":/images/new.png"),tr("&New task"), this);
	addTaskAct->setShortcut(tr("Ctrl+N"));
	addTaskAct->setStatusTip(tr("New task"));
	connect(addTaskAct, SIGNAL(triggered()), this, SLOT(newTask()));

	runTaskAct = new QAction(QIcon(":/images/run.png"),tr("&Run task"), this);
	runTaskAct->setShortcut(tr("Ctrl+R"));
	runTaskAct->setStatusTip(tr("Run task"));
	connect(runTaskAct, SIGNAL(triggered()), this, SLOT(runTask()));

	runAllTaskAct = new QAction(tr("Run all task"), this);
	runAllTaskAct->setStatusTip(tr("Run all task"));
	connect(runAllTaskAct, SIGNAL(triggered()), this, SLOT(runAllTask()));

	stopTaskAct = new QAction(QIcon(":/images/stop.png"),tr("&Stop task"), this);
	stopTaskAct->setShortcut(tr("Ctrl+S"));
	stopTaskAct->setStatusTip(tr("Stop task"));
	connect(stopTaskAct, SIGNAL(triggered()), this, SLOT(stopTask()));

	stopAllTaskAct = new QAction(tr("Stop all task"), this);
	stopAllTaskAct->setStatusTip(tr("Stop all task"));
	connect(stopAllTaskAct, SIGNAL(triggered()), this, SLOT(stopAllTask()));

	deleteTaskAct = new QAction(QIcon(":/images/delete.png"),tr("&Delete task"), this);
	deleteTaskAct->setShortcut(tr("Ctrl+D"));
	deleteTaskAct->setStatusTip(tr("Delete task"));
	connect(deleteTaskAct, SIGNAL(triggered()), this, SLOT(deleteTask()));

	redownloadTaskAct = new QAction(QIcon(":/images/redownload.png"),tr("D&ownload task again"), this);
	redownloadTaskAct->setShortcut(tr("Ctrl+O"));
	redownloadTaskAct->setStatusTip(tr("Download task again"));
	connect(redownloadTaskAct, SIGNAL(triggered()), this, SLOT(redownloadTask()));

	modifyTaskAct = new QAction(QIcon(":/images/modify.png"),tr("&Modify task properties"), this);
	modifyTaskAct->setShortcut(tr("Ctrl+M"));
	modifyTaskAct->setStatusTip(tr("Modify task properties"));
	connect(modifyTaskAct, SIGNAL(triggered()), this, SLOT(modifyTask()));

	viewDropWindowAct = new QAction(tr("D&rop window"), this);
	viewDropWindowAct->setShortcut(tr("Ctrl+R"));
	viewDropWindowAct->setStatusTip(tr("Show drop window"));
	viewDropWindowAct->setCheckable(true);
	connect(viewDropWindowAct, SIGNAL(triggered()), this, SLOT(viewDropWindowTriggered()));

	viewSpeedWidgetDockAct = new QAction(tr("Speed &widget"), this);
	viewSpeedWidgetDockAct->setShortcut(tr("Ctrl+W"));
	viewSpeedWidgetDockAct->setStatusTip(tr("Show speed widget"));
	viewSpeedWidgetDockAct->setCheckable(true);
	connect(viewSpeedWidgetDockAct, SIGNAL(triggered()), this, SLOT(viewSpeedWidgetDockTriggered()));

	viewTaskToolBarAct = new QAction(tr("&Task tool bar"), this);
	viewTaskToolBarAct->setShortcut(tr("Ctrl+T"));
	viewTaskToolBarAct->setStatusTip(tr("Show task tool bar"));
	viewTaskToolBarAct->setCheckable(true);
	connect(viewTaskToolBarAct, SIGNAL(triggered()), this, SLOT(viewTaskToolBarTriggered()));

	preferencesTaskAct = new QAction(QIcon(":/images/preferences.png"),tr("&Preferences"), this);
	preferencesTaskAct->setShortcut(tr("Ctrl+P"));
	preferencesTaskAct->setStatusTip(tr("System Configuration"));
	connect(preferencesTaskAct, SIGNAL(triggered()), this, SLOT(preferencesTriggered()));

	monitorClipboardAct = new QAction(tr("&Monitor clipboard"), this);
	monitorClipboardAct->setStatusTip(tr("Monitor clipboard"));
	monitorClipboardAct->setCheckable(true);
	connect(monitorClipboardAct, SIGNAL(triggered()), this, SLOT(monitorClipboardTriggered()));

	aboutAct = new QAction(QIcon(":/images/about.png"),tr("&About"), this);
	aboutAct->setShortcut(tr("Ctrl+A"));
	aboutAct->setStatusTip(tr("Show the application's About box"));
	connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

	aboutQtAct = new QAction(tr("About &Qt"), this);
	aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
	connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

	restoreAct = new QAction(tr("&Restore"), this);
	connect(restoreAct, SIGNAL(triggered()), this, SLOT(showApplicationNormal()));

	hideAct = new QAction(tr("&Hide"), this);
	connect(hideAct, SIGNAL(triggered()), this, SLOT(hide()));

	copyTaskThreadsDetailAct = new QAction(tr("&Copy"), this);
	connect(copyTaskThreadsDetailAct, SIGNAL(triggered()), this, SLOT(copyTaskThreadsDetail()));

	separatorAct = new QAction(this);
	separatorAct->setSeparator(true);

	separatorAct2 = new QAction(this);
	separatorAct2->setSeparator(true);

	separatorAct3 = new QAction(this);
	separatorAct3->setSeparator(true);
#if defined(DEBUG)
	testAct = new QAction(tr("Test"), this);
	connect(testAct, SIGNAL(triggered()), this, SLOT(testTriggered()));
#endif
}
void MainWindow::createTrayIcon()
{
	trayIconMenu = new QMenu(this);

	trayIconMenu->addAction(addTaskAct);
	trayIconMenu->addSeparator();
	trayIconMenu->addAction(runAllTaskAct);
	trayIconMenu->addAction(stopAllTaskAct);
	trayIconMenu->addSeparator();
	trayIconMenu->addAction(preferencesTaskAct);
	trayIconMenu->addAction(monitorClipboardAct);
	trayIconMenu->addSeparator();
	trayIconMenu->addAction(restoreAct);
	trayIconMenu->addAction(hideAct);
	trayIconMenu->addSeparator();
	trayIconMenu->addAction(aboutAct);
	trayIconMenu->addAction(quitAct);

	trayIcon = new QSystemTrayIcon(this);
	trayIcon->setContextMenu(trayIconMenu);
	QIcon icon = QIcon(":/images/icon.png");
	setWindowIcon(icon);
	trayIcon->setIcon(icon);
	trayIcon->setToolTip(tr("NetFleet"));
	trayIcon->show();

	connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));
}
void MainWindow::createMenus()
{
	fileMenu = new QMenu(tr("&File"));
	fileMenu->addAction(quitAct);

	taskMenu = new QMenu(tr("&Task"));
	taskMenu->addAction(addTaskAct);
	taskMenu->addAction(runTaskAct);
	taskMenu->addAction(stopTaskAct);
	taskMenu->addAction(deleteTaskAct);
	taskMenu->addSeparator();
	taskMenu->addAction(modifyTaskAct);
	taskMenu->addAction(redownloadTaskAct);
	taskMenu->addSeparator();
	taskMenu->addAction(runAllTaskAct);
	taskMenu->addAction(stopAllTaskAct);

	viewMenu = new QMenu(tr("&View"));
	createLanguageMenu();
	viewMenu->addMenu(languageSubMenu);
	viewMenu->addAction(viewDropWindowAct);
	viewMenu->addAction(viewSpeedWidgetDockAct);
	viewMenu->addAction(viewTaskToolBarAct);

	optionMenu = new QMenu(tr("&Option"));
	optionMenu->addAction(preferencesTaskAct);
	optionMenu->addAction(monitorClipboardAct);

	helpMenu = new QMenu(tr("&Help"));
	helpMenu->addAction(aboutAct);
//	helpMenu->addAction(aboutQtAct);

	menuBar = new QMenuBar(0);
	menuBar->addMenu(fileMenu);
	menuBar->addMenu(taskMenu);
	menuBar->addMenu(viewMenu);
	menuBar->addMenu(optionMenu);
	menuBar->addMenu(helpMenu);

	setMenuBar(menuBar);
}
void MainWindow::createLanguageMenu()
{
	languageSubMenu = new QMenu(tr("&Language"));

	languageActionGroup = new QActionGroup(this);
	connect(languageActionGroup, SIGNAL(triggered(QAction *)), this, SLOT(switchLanguage(QAction *)));

	QDir qmDir(":/translations");
	QStringList fileNames = qmDir.entryList(QStringList("netFleet_*.qm"));

	QSettings settings;
	settings.beginGroup("MainWindow");
	QString usingLanguage = settings.value("language").toString();
	settings.endGroup();
	QTranslator usingTranslator;
	if (usingLanguage == "")
		usingTranslator.load(":/translations/netFleet_" + QLocale::system().name());
	else
		usingTranslator.load(":/translations/netFleet_" + usingLanguage);
	usingLanguage = usingTranslator.translate("MainWindow", "English");

	for (int i = 0; i < fileNames.size(); ++i)
	{
		QString locale = fileNames[i];
		locale.remove(0, locale.indexOf('_') + 1);
		locale.chop(3);

		QTranslator translator;
		translator.load(fileNames[i], qmDir.absolutePath());
		QString language = translator.translate("MainWindow", "English");

		QAction *action = new QAction(tr("&%1 %2").arg(i + 1).arg(language), this);
		action->setCheckable(true);
		action->setData(locale);

		languageSubMenu->addAction(action);
		languageActionGroup->addAction(action);

		if (language == usingLanguage) action->setChecked(true);
	}
}
void MainWindow::createToolBars()
{
	taskToolBar = new QToolBar(tr("Task"));
	taskToolBar->setObjectName("TaskToolBar");
	taskToolBar->addAction(addTaskAct);
	taskToolBar->addAction(runTaskAct);
	taskToolBar->addAction(stopTaskAct);
	taskToolBar->addAction(deleteTaskAct);
	taskToolBar->addSeparator();
	taskToolBar->addAction(modifyTaskAct);
	taskToolBar->addAction(redownloadTaskAct);
	taskToolBar->addSeparator();
	taskToolBar->addAction(preferencesTaskAct);
	taskToolBar->addSeparator();
	taskToolBar->addAction(quitAct);
#if defined(DEBUG)
	taskToolBar->addAction(testAct);
#endif

	addToolBar(taskToolBar);
}
void MainWindow::createSmallWindow()
{
	smallWindow = new SmallWindow(this);
	smallWindow->setData(taskPreferences.maxSpeedGraph*1024, taskPreferences.pixelWidthGraph);
	smallWindowTaskInfoListId = 0;
	smallWindowShowSeconds = 0;
	showSmallWindow();

	connect(smallWindow, SIGNAL(addNewTask(QString)), this, SLOT(newTask(QString)));
}
void MainWindow::createSpeedWidgetDock()
{
	speedWidgetDock = new QDockWidget(tr("Speed dock"), this);
	speedWidgetDock->setObjectName("Dock");
	speedWidgetDock->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
	speedGraphWidget = new SpeedGraphWidget();
	speedWidgetDock->setWidget(speedGraphWidget);
	addDockWidget(Qt::TopDockWidgetArea, speedWidgetDock);
}
void MainWindow::createTaskTableWidget()
{
	tableWidget = new QTableWidget(this);
	tableWidget->setColumnCount(11);
	tableWidget->setRowCount(0);
	tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	QStringList tableHeaders;
	tableHeaders << tr("State") << tr("File name") << tr("Size") << tr("Compeleted") << tr("Progress") << tr("Speed") << tr("Elapsed time")<< tr("Time left") << tr("Retry") << tr("Url") << tr("Comment");
	tableWidget->setHorizontalHeaderLabels(tableHeaders);
	tableWidget->verticalHeader()->hide();
	tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
	tableWidget->horizontalHeader()->setHighlightSections(false);
	tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	tableWidget->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
	tableWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	tableWidget->setFocusPolicy(Qt::NoFocus);
	QList <int> columnWidths;
	columnWidths<<25<<130<<90<<80<<70<<70<<70<<70<<40<<300<<500;
	for (int i=0;i<columnWidths.size();i++)
	{
		tableWidget->setColumnWidth(i,columnWidths.at(i));
	}

	connect(tableWidget, SIGNAL(itemSelectionChanged()), this, SLOT(taskTableWidgetSelectionChanged()) );
	connect(tableWidget, SIGNAL(cellDoubleClicked(int , int)), this, SLOT(taskTableWidgetDoubleClicked(int , int)) );
}
void MainWindow::createThreadNumberTreeWidget()
{
	threadNumberTreeWidget = new QTreeWidget();
	threadNumberTreeWidget->setHeaderHidden(true);
//	threadNumberTreeWidget->setColumnCount(2);
	threadNumberTreeWidget->setFocusPolicy(Qt::NoFocus);

	threadInfomationTreeWidgetItem = new QTreeWidgetItem(threadNumberTreeWidget);
	threadInfomationTreeWidgetItem->setText(0, "Infomation");

	connect(threadNumberTreeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(threadNumberSelectionChanged()) );
}
void MainWindow::createTaskGraphPainterWidget()
{
	taskGraphPainterWidget = new TaskGraphPainterWidget();
	taskGraphWidget = new QScrollArea();
	taskGraphWidget->setWidget(taskGraphPainterWidget);
	taskGraphWidget->setWidgetResizable(true);
	taskGraphWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	taskGraphWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	taskGraphWidget->viewport()->setBackgroundRole(QPalette::NoRole);
	taskGraphWidget->setFrameStyle(QFrame::NoFrame);
	taskGraphWidget->setFocusPolicy(Qt::NoFocus);
}
void MainWindow::createThreadsDetailTableWidget()
{
	threadsDetailTableWidget = new QTableWidget(this);
	threadsDetailTableWidget->setColumnCount(2);
	threadsDetailTableWidget->setFocusPolicy(Qt::NoFocus);
	QStringList tableHeaders;
	tableHeaders << tr("Time") << tr("Infomation");
	threadsDetailTableWidget->setHorizontalHeaderLabels(tableHeaders);
	threadsDetailTableWidget->verticalHeader()->hide();
	threadsDetailTableWidget->horizontalHeader()->setHighlightSections(false);
//	threadsDetailTableWidget->horizontalHeader()->hide();
	threadsDetailTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	threadsDetailTableWidget->setContentsMargins(0,0,0,0);
	threadsDetailTableWidget->setShowGrid(false);
	threadsDetailTableWidget->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
	threadsDetailTableWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	threadsDetailTableWidget->setAutoScroll(true);
//	threadsDetailTableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}
void MainWindow::createTaskInfomation()
{
	taskInfoList.clear();
	int taskInfoListId = 0;
	while ( true )
	{
		QSettings settings("NetFleet","task");
		settings.beginGroup(QString("task%1").arg(taskInfoListId));
		QString urlString = settings.value("url").toString();
		QString referrerString  = settings.value("referrer").toString();
		QString saveToString = settings.value("saveTo").toString();
		QString renameString = settings.value("rename").toString();
		int splitInt = settings.value("split").toInt();
		QString userString = settings.value("user").toString();
		QString passwordString = settings.value("password").toString();
		QString commentString = settings.value("comment").toString();
		QList <QVariant> urlsListVariant = settings.value("urls").toList();
		settings.endGroup();
		if (urlString.isEmpty()) break;

		TaskInfomation *taskInfo;
		taskInfo = new TaskInfomation;
		taskInfo->taskInfoListId = taskInfoListId;
		taskInfo->controlState = noState;
		taskInfo->state = stopState;
		taskInfo->url.setUrl(urlString);
		taskInfo->referrer = referrerString;
		taskInfo->saveTo = saveToString;
		taskInfo->rename = renameString;
		taskInfo->comment = commentString;
		taskInfo->splite = splitInt;
		taskInfo->retry = 0;
		taskInfo->size = 0;
		taskInfo->completed = 0;
		taskInfo->lastCompleted = 0;
		taskInfo->speed = 0;
		taskInfo->elapsedTime = 0;
		foreach( QVariant url, urlsListVariant )
		taskInfo->urlsList<< url.toString();

		taskInfoList.append(taskInfo);

		readTaskState(taskInfoListId);
		qint64 taskInfoListIdNotCompleted = 0;
		for (int notDownloadListId=0;notDownloadListId<taskInfoList.at(taskInfoListId)->notDownloadList.size();notDownloadListId++)
		{
			taskInfoListIdNotCompleted += taskInfoList.at(taskInfoListId)->notDownloadList.at(notDownloadListId)->endPosition - taskInfoList.at(taskInfoListId)->notDownloadList.at(notDownloadListId)->startPosition;
		}
		taskInfoList.at(taskInfoListId)->completed = taskInfoList.at(taskInfoListId)->size - taskInfoListIdNotCompleted;
		taskInfoList.at(taskInfoListId)->lastCompleted = taskInfoList.at(taskInfoListId)->completed;

		int row = tableWidget->rowCount();
		tableWidget->insertRow(row);
		if (taskInfoList.at(taskInfoListId)->state == runningState)
			taskInfoList.at(taskInfoListId)->state = stopState;
		showTaskState(taskInfoListId);
		tableWidget->setItem(taskInfoListId, fileNameHeader, new QTableWidgetItem(taskInfoList.at(taskInfoListId)->rename));
		tableWidget->setItem(taskInfoListId, sizeHeader, new QTableWidgetItem(QString("%1").arg(taskInfoList.at(taskInfoListId)->size)));
		tableWidget->setItem(taskInfoListId, compeletedHeader, new QTableWidgetItem(QString("%1").arg(taskInfoList.at(taskInfoListId)->completed)));
		float percent = 0;
		if (taskInfoList.at(taskInfoListId)->size != 0)
			percent=float(taskInfoList.at(taskInfoListId)->completed)/float(taskInfoList.at(taskInfoListId)->size)*100;
		tableWidget->setItem(taskInfoListId, progressHeader, new QTableWidgetItem(QString("%1").arg(percent,0,'f',2)));
		tableWidget->setItem(taskInfoListId, urlHeader, new QTableWidgetItem(taskInfoList.at(taskInfoListId)->url.toString()));
		tableWidget->setItem(taskInfoListId, commentHeader, new QTableWidgetItem(taskInfoList.at(taskInfoListId)->comment));

		taskInfoListId++;
	}
	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(showTaskMessages()) );
	timer->start(1000);
}
void MainWindow::about()
{
	showApplicationNormal();
	QDialog *about = new QDialog(this);
	Ui::About ui;
	ui.setupUi( about );
	about->exec();
}
void MainWindow::newTask(QString urlString)
{
	showApplicationNormal();
	int taskInfoListId = taskInfoList.size();
	NewTaskDialog *newTaskDialog = new NewTaskDialog(urlString, this);

	if ( newTaskDialog->exec() == QDialog::Accepted )
	{
		TaskInfomation *taskInfo;
		taskInfo = new TaskInfomation;
		taskInfo->taskInfoListId = taskInfoList.size();
		taskInfo->url.setUrl(newTaskDialog->urlLineEdit->text());
		taskInfo->referrer = newTaskDialog->referrerLineEdit->text();
		taskInfo->saveTo = newTaskDialog->saveToLineEdit->text();
		taskInfo->rename = newTaskDialog->renameLineEdit->text();
		taskInfo->splite = newTaskDialog->splitSpinBox->value();
		taskInfo->user = newTaskDialog->userLineEdit->text();
		taskInfo->password = newTaskDialog->passwordLineEdit->text();

		bool startImmediately;
		if ( newTaskDialog->immediatelyRadioButton->isChecked() )
			startImmediately = true;
		else
			startImmediately = false;

		taskInfo->comment = newTaskDialog->commentTextEdit->toPlainText();

		QList<QString> urlsList;
		for (int row=0;row<newTaskDialog->ulrsTableWidget->rowCount();row++)
		{
			QTableWidgetItem *item = newTaskDialog->ulrsTableWidget->item(row,1);
			QString urlString = item->text();
			urlsList<<urlString;
		}
		taskInfo->urlsList = urlsList;

		taskInfo->controlState = noState;
		taskInfo->state = stopState;
		taskInfo->retry = 0;
		taskInfo->size = 0;
		taskInfo->completed = 0;
		taskInfo->lastCompleted = 0;
		taskInfo->speed = 0;
		taskInfo->elapsedTime = 0;
		taskInfoList.append(taskInfo);

		qint64 taskInfoListIdNotCompleted = 0;
		for (int notDownloadListId=0;notDownloadListId<taskInfoList.at(taskInfoListId)->notDownloadList.size();notDownloadListId++)
		{
			taskInfoListIdNotCompleted += taskInfoList.at(taskInfoListId)->notDownloadList.at(notDownloadListId)->endPosition - taskInfoList.at(taskInfoListId)->notDownloadList.at(notDownloadListId)->startPosition;
		}
		taskInfoList.at(taskInfoListId)->completed = taskInfoList.at(taskInfoListId)->size - taskInfoListIdNotCompleted;

		int row = tableWidget->rowCount();
		tableWidget->insertRow(row);
		if (taskInfoList.at(taskInfoListId)->state == runningState)
			taskInfoList.at(taskInfoListId)->state = stopState;
		showTaskState(taskInfoListId);
		tableWidget->setItem(taskInfoListId, fileNameHeader, new QTableWidgetItem(taskInfoList.at(taskInfoListId)->rename));
		tableWidget->setItem(taskInfoListId, sizeHeader, new QTableWidgetItem(QString("%1").arg(taskInfoList.at(taskInfoListId)->size)));
		tableWidget->setItem(taskInfoListId, compeletedHeader, new QTableWidgetItem(QString("%1").arg(taskInfoList.at(taskInfoListId)->completed)));
		tableWidget->setItem(taskInfoListId, urlHeader, new QTableWidgetItem(taskInfoList.at(taskInfoListId)->url.toString()));

		QSettings settings("NetFleet","task");
		settings.beginGroup(QString("task%1").arg(taskInfoListId));
		settings.setValue("url", taskInfo->url.toString());
		settings.setValue("referrer", taskInfo->referrer);
		settings.setValue("saveTo", taskInfo->saveTo);
		settings.setValue("rename", taskInfo->rename);

		settings.setValue("split", taskInfo->splite);
		settings.setValue("user", taskInfo->user);
		settings.setValue("password", taskInfo->password);

		settings.setValue("comment", taskInfo->comment);

		QList <QVariant> urlsListVariant;
		foreach( QString url, urlsList )
		urlsListVariant << url;
		settings.setValue("urls", urlsListVariant);
		settings.endGroup();

		QSettings settings2;
		settings2.beginGroup("savePath");
		settings2.setValue("lastPath", taskInfo->saveTo);
		settings2.endGroup();

		if (startImmediately) runTask(taskInfoListId);
	}

}
void MainWindow::quitTriggered()
{
	bool runningTaskBool = false;
	for (int taskInfoListId=0;taskInfoListId<taskInfoList.size();taskInfoListId++)
	{
		if (taskInfoList.at(taskInfoListId)->state == runningState) runningTaskBool = true;
	}

	if (runningTaskBool)
	{
//		QMessageBox::information(this, tr("Infomation"), tr("There are tasks are running.Close Window Will stop all running task.Quit?"));
		if (QMessageBox::question(this, tr("Question"),tr("There are tasks are running.Close Window Will stop all running task.Quit?"),
		                          QMessageBox::Ok|QMessageBox::Cancel, QMessageBox::Cancel)
		    == QMessageBox::Cancel)
		{
			return;
		}
		else
		{
			stopAllTask();
			if (quitApplicationTimer == 0)
			{
				quitApplicationTimer = new QTimer(this);
				connect(quitApplicationTimer, SIGNAL(timeout()), this, SLOT(quitApplication()));
				quitApplicationTimer->start(50);
				return;
			}
		}
	}
	for (int taskInfoListId=0;taskInfoListId<taskInfoList.size();taskInfoListId++)
	{
		if (taskInfoList.at(taskInfoListId)->state == pauseState) taskInfoList.at(taskInfoListId)->state=stopState;
		saveTaskState(taskInfoListId);
	}
	saveSettings();
	qApp->quit();
}
#if defined(DEBUG)
void MainWindow::testTriggered()
{
	int taskInfoListId = getCurrentTaskInfoListId();
	if (taskInfoListId == -1) return;

	QFile file("out.txt");
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
		return;

	QTextStream out(&file);
	out << "notDownloadList: " << "\n";
	for (int notDownloadListId=0;notDownloadListId<taskInfoList.at(taskInfoListId)->notDownloadList.size();notDownloadListId++)
	{
		out << "notDownloadListId: " << notDownloadListId
		<< ",taskThreadListId:" << taskInfoList.at(taskInfoListId)->notDownloadList.at(notDownloadListId)->taskThreadListId
		<< ",startPosition:" << taskInfoList.at(taskInfoListId)->notDownloadList.at(notDownloadListId)->startPosition
		<< ",endPosition" <<taskInfoList.at(taskInfoListId)->notDownloadList.at(notDownloadListId)->endPosition
		<< "\n";
	}
	out << "\ntaskThreadList: " << "\n";
	for (int taskThreadListId=0;taskThreadListId<taskInfoList.at(taskInfoListId)->taskThreadList.size();taskThreadListId++)
	{
		out << "taskThreadListId: " << taskThreadListId
		<< ",startPosition:" << taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->threadTaskInfomation.startPosition
		<< ",completed:" << taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->threadTaskInfomation.completed
		<< ",threadStopped" << taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->threadStopped
		<< ",httpRequestFinished" <<taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->threadTaskInfomation.httpRequestFinished
		<< "\n";
	}
	file.close();
}
#endif
void MainWindow::quitApplication()
{
	bool runningTaskBool = false;
	for (int taskInfoListId=0;taskInfoListId<taskInfoList.size();taskInfoListId++)
	{
		if (taskInfoList.at(taskInfoListId)->state == runningState) runningTaskBool = true;
	}

	if (!runningTaskBool)
	{
		quitApplicationTimer->stop();
		delete quitApplicationTimer;
		quitApplicationTimer = 0;

		for (int taskInfoListId=0;taskInfoListId<taskInfoList.size();taskInfoListId++)
		{
			if (taskInfoList.at(taskInfoListId)->state == pauseState) taskInfoList.at(taskInfoListId)->state=stopState;
			saveTaskState(taskInfoListId);
		}
		saveSettings();
		qApp->quit();
	}
}
void MainWindow::runTask(int taskInfoListId)
{
	if (taskInfoListId == -1) taskInfoListId = getCurrentTaskInfoListId();
	if (taskInfoListId == -1) return;
	if (taskInfoList.at(taskInfoListId)->state == finishedState)
		return;

	if (getRunningTasksCount() >= taskPreferences.maxRunningTasksInt)
	{
		taskInfoList.at(taskInfoListId)->state = pauseState;
		return;
	}

//	threadsDetailTableWidgetClear();

	taskInfoList.at(taskInfoListId)->state = runningState;
	taskInfoList.at(taskInfoListId)->retry = 0;
	taskInfoList.at(taskInfoListId)->elapsedTime = 0;
	//resume task
	if (!taskInfoList.at(taskInfoListId)->notDownloadList.isEmpty() && taskInfoList.at(taskInfoListId)->size>0)
	{
		int threadsCount = 0;
		if (taskInfoList.at(taskInfoListId)->notDownloadList.size() > taskInfoList.at(taskInfoListId)->splite)
			threadsCount = taskInfoList.at(taskInfoListId)->splite;
		else
			threadsCount = taskInfoList.at(taskInfoListId)->notDownloadList.size();
		int taskThreadListId = 0;
		for (int notDownloadListId=0;notDownloadListId<threadsCount;notDownloadListId++)
		{
			taskThreadListId = notDownloadListId;
			if (taskInfoList.at(taskInfoListId)->notDownloadList.at(notDownloadListId)->taskThreadListId == -1)
			{
				downloadThread = new DownloadThread();
				taskInfoList.at(taskInfoListId)->taskThreadList.append(downloadThread);
				taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->threadTaskInfomation.newReceivedList.clear();
				threadNewReceivedListAppend(taskInfoListId, taskThreadListId, notDownloadListId);
				startTaskThread(taskInfoListId, taskThreadListId, notDownloadListId, true);
			}

		}
		taskThreadListId++;
		while (taskThreadListId < taskInfoList.at(taskInfoListId)->splite)
		{
			if (newNotDownload(taskInfoListId) == false) break;

			downloadThread = new DownloadThread();
			taskInfoList.at(taskInfoListId)->taskThreadList.append(downloadThread);
#if defined(DEBUG)
			qDebug()<<qPrintable(QString("start new thread:%1").arg(taskThreadListId));
#endif
			taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->threadTaskInfomation.newReceivedList.clear();
			threadNewReceivedListAppend(taskInfoListId, taskThreadListId, taskInfoList.at(taskInfoListId)->notDownloadList.size()-1);

			startTaskThread(taskInfoListId, taskThreadListId, taskInfoList.at(taskInfoListId)->notDownloadList.size()-1, true);
			taskThreadListId++;
		}
		taskTableWidgetSelectionChanged();

		return;
	}

	//start new download
	QString taskFileName = QString("%1/%2").arg(taskInfoList.at(taskInfoListId)->saveTo).arg(taskInfoList.at(taskInfoListId)->rename);
	int n = taskFileName.lastIndexOf("?");
	taskFileName = taskFileName.left(n);
	if (QFile::exists(taskFileName))
	{
		if (QMessageBox::question(this, tr("Infomation"),
		                          tr("There already exists a file called %1 in "
		                             "the current directory. Overwrite?").arg(taskFileName),
		                          QMessageBox::Ok|QMessageBox::Cancel, QMessageBox::Cancel)
		    == QMessageBox::Cancel)
		{
			taskInfoList.at(taskInfoListId)->state = stopState;
			return;
		}
		QFile::remove(taskFileName);
	}
	taskInfoList.at(taskInfoListId)->elapsedTime = 0;

	newNotDownload(taskInfoListId);
	taskInfoList.at(taskInfoListId)->notDownloadList.at(taskInfoList.at(taskInfoListId)->notDownloadList.size()-1)->taskThreadListId = 0;
	downloadThread = new DownloadThread();
	taskInfoList.at(taskInfoListId)->taskThreadList.append(downloadThread);
	taskInfoList.at(taskInfoListId)->taskThreadList.at(0)->threadTaskInfomation.newReceivedList.clear();
	threadNewReceivedListAppend(taskInfoListId, 0, taskInfoList.at(taskInfoListId)->notDownloadList.size()-1);
	startTaskThread(taskInfoListId, 0, 0, true);
	taskTableWidgetSelectionChanged();
}
void MainWindow::runAllTask()
{
	for (int taskInfoListId=0;taskInfoListId<taskInfoList.size();taskInfoListId++)
	{
		if (taskInfoList.at(taskInfoListId)->state==stopState || taskInfoList.at(taskInfoListId)->state==errorState)
			runTask(taskInfoListId);
	}
}
void MainWindow::showTaskMessages()
{
	qint64 totalSpeed = 0;
	for (int taskInfoListId=0;taskInfoListId<taskInfoList.size();taskInfoListId++)
	{
		checkNotDownloadList(taskInfoListId);

		//get file completed bytes
		qint64 taskInfoListIdNotCompleted = 0;
		for (int notDownloadListId=0;notDownloadListId<taskInfoList.at(taskInfoListId)->notDownloadList.size();notDownloadListId++)
		{
			taskInfoListIdNotCompleted += taskInfoList.at(taskInfoListId)->notDownloadList.at(notDownloadListId)->endPosition - taskInfoList.at(taskInfoListId)->notDownloadList.at(notDownloadListId)->startPosition;
		}

		taskInfoList.at(taskInfoListId)->completed = taskInfoList.at(taskInfoListId)->size - taskInfoListIdNotCompleted;

		showTaskState(taskInfoListId);
		saveTaskState(taskInfoListId);
		showActionState();
		checkTaskThread(taskInfoListId);

		//get elapsed time
		if (taskInfoList.at(taskInfoListId)->state == runningState)
		{
			taskInfoList.at(taskInfoListId)->elapsedTime++;
		}

		//get speed
		qint64 speed = 0;
		if (taskInfoList.at(taskInfoListId)->state == runningState)
			speed = taskInfoList.at(taskInfoListId)->completed - taskInfoList.at(taskInfoListId)->lastCompleted;
		if (speed < 0) speed = 0;
		taskInfoList.at(taskInfoListId)->speed = taskInfoList.at(taskInfoListId)->completed - taskInfoList.at(taskInfoListId)->lastCompleted;
		taskInfoList.at(taskInfoListId)->lastCompleted = taskInfoList.at(taskInfoListId)->completed;
		totalSpeed += speed;

		Tools tools;
		tableWidget->setItem(taskInfoListId, sizeHeader, new QTableWidgetItem(tools.get_size(taskInfoList.at(taskInfoListId)->size)));
		tableWidget->setItem(taskInfoListId, compeletedHeader, new QTableWidgetItem(tools.get_size(taskInfoList.at(taskInfoListId)->completed)));
		float percent = 0;
		if (taskInfoList.at(taskInfoListId)->size != 0)
			percent=float(taskInfoList.at(taskInfoListId)->completed)/float(taskInfoList.at(taskInfoListId)->size)*100;

		taskInfoList.at(taskInfoListId)->speedPointList<<taskInfoList.at(taskInfoListId)->speed;
		while (taskInfoList.at(taskInfoListId)->speedPointList.size() > 50)
			taskInfoList.at(taskInfoListId)->speedPointList.removeAt(0);

		tableWidget->setItem(taskInfoListId, progressHeader, new QTableWidgetItem(QString("%1").arg(percent,0,'f',2)));
		if (taskInfoList.at(taskInfoListId)->state == runningState)
		{
			tableWidget->setItem(taskInfoListId, speedHeader, new QTableWidgetItem(tools.get_size(speed)));
			tableWidget->setItem(taskInfoListId, elapsedTimeHeader, new QTableWidgetItem(tools.getTime(taskInfoList.at(taskInfoListId)->elapsedTime)));
			tableWidget->setItem(taskInfoListId, timeLeftHeader, new QTableWidgetItem(tools.getTimeLeft(taskInfoList.at(taskInfoListId)->size, taskInfoList.at(taskInfoListId)->completed, speed)));
		}
		else
		{
			tableWidget->setItem(taskInfoListId, speedHeader, new QTableWidgetItem(""));
			tableWidget->setItem(taskInfoListId, timeLeftHeader, new QTableWidgetItem(""));
		}
		tableWidget->setItem(taskInfoListId, retryHeader, new QTableWidgetItem(QString("%1").arg(taskInfoList.at(taskInfoListId)->retry)));
	}

	speedGraphWidget->addSpeedPoint(totalSpeed);
	showSmallWindow();
	showGraphWidget(false);
}
void MainWindow::showSmallWindow()
{
	if (smallWindowTaskInfoListId>=taskInfoList.size())
		smallWindowTaskInfoListId = 0;
	int taskInfoListId = smallWindowTaskInfoListId;
	while (true)
	{
		if (smallWindowTaskInfoListId>=0 && smallWindowTaskInfoListId<taskInfoList.size())
		{
			if (taskInfoList.at(smallWindowTaskInfoListId)->state == runningState)
			{
				float percent = 0;
				if (taskInfoList.at(smallWindowTaskInfoListId)->size != 0)
					percent=float(taskInfoList.at(smallWindowTaskInfoListId)->completed)/float(taskInfoList.at(smallWindowTaskInfoListId)->size)*100;
				QString showString = QString("%1%").arg(percent,0,'f',1);
				smallWindow->setString(showString);
				smallWindow->setSpeedPoint(taskInfoList.at(smallWindowTaskInfoListId)->speedPointList);
				smallWindowShowSeconds++;
				if (smallWindowShowSeconds == 3)
				{
					smallWindowTaskInfoListId++;
					if (smallWindowTaskInfoListId>=taskInfoList.size())
						smallWindowTaskInfoListId = 0;
					smallWindowShowSeconds = 0;
				}
				break;
			}
		}

		smallWindowTaskInfoListId++;

		if (smallWindowTaskInfoListId >= taskInfoList.size() )
			smallWindowTaskInfoListId = 0;
		if (smallWindowTaskInfoListId == taskInfoListId)
		{
			smallWindow->refreshIcon();
			break;
		}
	}
}
void MainWindow::checkTaskThread(int taskInfoListId)
{
	//start pause state task
	if (taskInfoList.at(taskInfoListId)->state==pauseState && getRunningTasksCount()<taskPreferences.maxRunningTasksInt)
	{
		runTask(taskInfoListId);
	}

	//start finished and error threads.
	for (int taskThreadListId=0;taskThreadListId<taskInfoList.at(taskInfoListId)->taskThreadList.size();taskThreadListId++)
	{
		if (taskInfoList.at(taskInfoListId)->controlState != noState) break;
		if (taskInfoList.at(taskInfoListId)->state==runningState && taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->threadTaskInfomation.httpRequestFinished==true && taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->threadStopped==true && taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->isRunning()==false)
		{
			startFinishedThread(taskInfoListId, taskThreadListId);
		}

		if (taskInfoList.at(taskInfoListId)->state==runningState && taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->threadStopped==true && taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->isRunning()==false && taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->threadError==true && taskInfoList.at(taskInfoListId)->retry<taskPreferences.maxRetryNumberInt)
		{
#if defined(DEBUG)
			qDebug()<<"start error thread";
#endif
			taskInfoList.at(taskInfoListId)->retry++;
			taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->threadTaskInfomation.startPosition += taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->threadTaskInfomation.completed;
			taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->threadTaskInfomation.completed = 0;
			taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->threadStopped = false;
			taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->start();
		}

		/*		if (taskInfoList.at(taskInfoListId)->state==runningState && taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->threadStopped==true && taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->isRunning()==false && taskInfoList.at(taskInfoListId)->retry<taskPreferences.maxRetryNumberInt)
				{
					if (taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->threadTaskInfomation.httpRequestFinished==false && taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->threadError==false)
					{
						for (int notDownloadListId=0;notDownloadListId<taskInfoList.at(taskInfoListId)->notDownloadList.size();notDownloadListId++)
						{
							if (taskInfoList.at(taskInfoListId)->notDownloadList.at(notDownloadListId)->taskThreadListId == taskThreadListId)
								if (taskInfoList.at(taskInfoListId)->notDownloadList.at(notDownloadListId)->startPosition < taskInfoList.at(taskInfoListId)->notDownloadList.at(notDownloadListId)->endPosition)
								{
		#if defined(DEBUG)
									qDebug()<<"start non-normal stopped thread";
		#endif
									taskInfoList.at(taskInfoListId)->retry++;
									taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->threadTaskInfomation.startPosition = taskInfoList.at(taskInfoListId)->notDownloadList.at(notDownloadListId)->startPosition;
									taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->threadTaskInfomation.completed = 0;
									taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->threadStopped = false;
									taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->start();
								}
						}
					}
				}
		*/
		if (taskInfoList.at(taskInfoListId)->state==runningState && taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->threadStopped==true && taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->isRunning()==false && taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->threadError==true && taskInfoList.at(taskInfoListId)->retry==taskPreferences.maxRetryNumberInt)
		{
#if defined(DEBUG)
			qDebug()<<"errorState";
#endif
			taskInfoList.at(taskInfoListId)->controlState = errorState;
			deleteTaskList.append(taskInfoListId);

			taskTableWidgetSelectionChanged();

			if (deleteTaskTimer == 0)
			{
				deleteTaskTimer = new QTimer(this);
				connect(deleteTaskTimer, SIGNAL(timeout()), this, SLOT(deleteTaskDownloadThread()));
				deleteTaskTimer->start(50);
			}
		}
	}
}
void MainWindow::checkNotDownloadList(int taskInfoListId)
{
	for (int notDownloadListId=0;notDownloadListId<taskInfoList.at(taskInfoListId)->notDownloadList.size();notDownloadListId++)
	{
		int taskThreadListId = taskInfoList.at(taskInfoListId)->notDownloadList.at(notDownloadListId)->taskThreadListId;

		if (taskThreadListId != -1)
		{
			taskInfoList.at(taskInfoListId)->notDownloadList.at(notDownloadListId)->startPosition = taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->threadTaskInfomation.startPosition + taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->threadTaskInfomation.completed;
			if (taskInfoList.at(taskInfoListId)->notDownloadList.at(notDownloadListId)->startPosition >taskInfoList.at(taskInfoListId)->notDownloadList.at(notDownloadListId)->endPosition)
			{
#if defined(DEBUG)
				qDebug()<<"if(startPosition>endPosition) startPosition=endPosition="<<taskInfoList.at(taskInfoListId)->notDownloadList.at(notDownloadListId)->endPosition;
#endif
				taskInfoList.at(taskInfoListId)->notDownloadList.at(notDownloadListId)->startPosition = taskInfoList.at(taskInfoListId)->notDownloadList.at(notDownloadListId)->endPosition;
			}
			qint64 startPosition;
			startPosition = taskInfoList.at(taskInfoListId)->notDownloadList.at(notDownloadListId)->startPosition;
			if (taskInfoList.at(taskInfoListId)->taskThreadList.size()>0)
			{
				if (taskThreadListId < taskInfoList.at(taskInfoListId)->taskThreadList.size())
					taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->threadTaskInfomation.newReceivedList.append(startPosition);
				while (taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->threadTaskInfomation.newReceivedList.size() > 3)
					taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->threadTaskInfomation.newReceivedList.removeAt(0);
			}
			if (startPosition>=taskInfoList.at(taskInfoListId)->notDownloadList.at(notDownloadListId)->endPosition && taskInfoList.at(taskInfoListId)->notDownloadList.at(notDownloadListId)->endPosition!=0)
			{

				qint64 size = taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->threadTaskInfomation.newReceivedList.size();
				if (size > 0)
				{
					taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->threadTaskInfomation.newReceivedList.removeAt(size-1);
					qint64 endPosition = taskInfoList.at(taskInfoListId)->notDownloadList.at(notDownloadListId)->endPosition;
					taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->threadTaskInfomation.newReceivedList.append(endPosition);
				}
				showGraphWidget(false, taskInfoListId, taskThreadListId);
				taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->threadTaskInfomation.newReceivedList.clear();
				taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->threadTaskInfomation.httpRequestFinished = true;

				NotDownload *notDownload=taskInfoList.at(taskInfoListId)->notDownloadList.at(notDownloadListId);
				taskInfoList.at(taskInfoListId)->notDownloadList.removeAt(notDownloadListId);
				delete notDownload;
			}
		}
	}
}
void MainWindow::showThreadMessages(int taskInfoListId, int taskThreadListId, int messagesType, QString strResponseHeader)
{
	showThreadMessagesMutex.lock();
	int currentTaskInfoListId = getCurrentTaskInfoListId();

	int currentTaskThreadListId = getCurrentTaskThreadListId();

	QDateTime dateTime = QDateTime::currentDateTime();
	QString str = dateTime.toString("yyyy-MM-dd hh:mm:ss");
	QStringList list = strResponseHeader.split("\r\n");
	for (int i=0;i<list.size();i++)
	{
		QString str = dateTime.toString("yyyy-MM-dd hh:mm:ss ");
		if (list.at(i).isEmpty())
		{
			continue;
		}

		QTableWidgetItem *threadStateDetailItem;
		QIcon stateIcon;
		switch (messagesType)
		{
		case uploadMessages:
			stateIcon.addFile(":/images/uploadmessages.png");
			break;
		case downloadMessages:
			stateIcon.addFile(":/images/downloadmessages.png");
			break;
		case infomationMessages:
			stateIcon.addFile(":/images/infomationmessages.png");
			break;
		case errorMessages:
			stateIcon.addFile(":/images/errormessages.png");
			break;
		default:
			stateIcon.addFile(":/images/errormessages.png");
			break;
		}
		taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->threadTaskInfomation.stateDetailList.append(stateIcon);
		taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->threadTaskInfomation.timeDetailList.append(str);
		taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->threadTaskInfomation.messageDetailList.append(list.at(i));

		if (currentTaskInfoListId==taskInfoListId && currentTaskThreadListId==taskThreadListId)
		{
			int row = threadsDetailTableWidget->rowCount();
			threadsDetailTableWidget->insertRow(row);
			threadStateDetailItem = new QTableWidgetItem(stateIcon,str);
			threadsDetailTableWidget->setItem(row, 0, threadStateDetailItem);
			QTableWidgetItem *threadMessageDetailItem;
			threadMessageDetailItem =  new QTableWidgetItem(list.at(i));
			threadsDetailTableWidget->setItem(row, 1, threadMessageDetailItem);
			QFontMetrics fm = threadsDetailTableWidget->fontMetrics();
			int pixelsHigh = fm.height();
			threadsDetailTableWidget->setRowHeight(row, pixelsHigh);
			threadsDetailTableWidget->resizeColumnToContents(0);
			threadsDetailTableWidget->setColumnWidth(1, threadGraphDetailWidget->width()-threadsDetailTableWidget->columnWidth(0)-38);
			threadsDetailTableWidget->scrollToBottom();
		}
	}
	showThreadMessagesMutex.unlock();
}
void MainWindow::getFileSize(int taskInfoListId, qint64 fileSize)
{
	taskInfoList.at(taskInfoListId)->size = fileSize;
	taskInfoList.at(taskInfoListId)->notDownloadList.at(0)->endPosition = taskInfoList.at(taskInfoListId)->size;

	//start new download threads
	while (taskInfoList.at(taskInfoListId)->size>0 && taskInfoList.at(taskInfoListId)->state==runningState && taskInfoList.at(taskInfoListId)->taskThreadList.size() < taskInfoList.at(taskInfoListId)->splite && newNotDownload(taskInfoListId)==true)
	{
		int taskThreadListId = taskInfoList.at(taskInfoListId)->taskThreadList.size();
		taskInfoList.at(taskInfoListId)->notDownloadList.at(taskInfoList.at(taskInfoListId)->notDownloadList.size()-1)->taskThreadListId = taskThreadListId;

		downloadThread = new DownloadThread();
		taskInfoList.at(taskInfoListId)->taskThreadList.append(downloadThread);
		taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->threadTaskInfomation.newReceivedList.clear();
		threadNewReceivedListAppend(taskInfoListId, taskThreadListId, taskInfoList.at(taskInfoListId)->notDownloadList.size()-1);
		startTaskThread(taskInfoListId, taskThreadListId, taskThreadListId, true);
		taskTableWidgetSelectionChanged();
	}
	showGraphWidget(true);
}
void MainWindow::getFileDownloadFinished(int taskInfoListId, int taskThreadListId)
{
	taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->threadTaskInfomation.httpRequestFinished = true;
	NotDownload *notDownload=taskInfoList.at(taskInfoListId)->notDownloadList.at(0);
	taskInfoList.at(taskInfoListId)->notDownloadList.removeAt(0);
	delete notDownload;
#if defined(DEBUG)
	qDebug()<<qPrintable(QString("notDownloadList.size%1").arg(taskInfoList.at(taskInfoListId)->notDownloadList.size()));
#endif
	showSmallWindow();
	taskInfoList.at(taskInfoListId)->state = finishedState;
	if (taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->threadError)
	{
		taskInfoList.at(taskInfoListId)->controlState = errorState;
		deleteTaskList.append(taskInfoListId);

		taskTableWidgetSelectionChanged();

		if (deleteTaskTimer == 0)
		{
			deleteTaskTimer = new QTimer(this);
			connect(deleteTaskTimer, SIGNAL(timeout()), this, SLOT(deleteTaskDownloadThread()));
			deleteTaskTimer->start(50);
		}
	}

	showTaskState(taskInfoListId);
}
void MainWindow::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
	switch (reason)
	{
	case QSystemTrayIcon::Trigger:
	case QSystemTrayIcon::DoubleClick:
		if (isVisible())
			hide();
		else
			showApplicationNormal();
		break;
	case QSystemTrayIcon::MiddleClick:
	default:
		;
	}
}
void MainWindow::stopTask()
{
	int taskInfoListId = getCurrentTaskInfoListId();
	if (taskInfoListId == -1) return;
	for (int taskThreadListId=0;taskThreadListId<taskInfoList.at(taskInfoListId)->taskThreadList.size();taskThreadListId++)
	{
		taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->threadTaskInfomation.httpRequestAborted = true;
		taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->setThreadStopped();
	}

	taskInfoList.at(taskInfoListId)->controlState = stopState;
	deleteTaskList.append(taskInfoListId);

#if defined(DEBUG)
	qDebug("stopstate********************");
	for (int j=0;j<taskInfoList.at(taskInfoListId)->notDownloadList.size();j++)
	{
		qint64 startPosition = taskInfoList.at(taskInfoListId)->notDownloadList.at(j)->startPosition;
		qint64 endPosition = taskInfoList.at(taskInfoListId)->notDownloadList.at(j)->endPosition;
		int listId = taskInfoList.at(taskInfoListId)->notDownloadList.at(j)->taskThreadListId;
		qDebug()<<qPrintable(QString("%1:%2---%3").arg(listId).arg(startPosition).arg(endPosition));
	}
#endif

	if (deleteTaskTimer == 0)
	{
		deleteTaskTimer = new QTimer(this);
		connect(deleteTaskTimer, SIGNAL(timeout()), this, SLOT(deleteTaskDownloadThread()));
		deleteTaskTimer->start(50);
	}
}
void MainWindow::stopAllTask()
{
	for (int taskInfoListId=0;taskInfoListId<taskInfoList.size();taskInfoListId++)
	{
		if ( taskInfoList.at(taskInfoListId)->state == pauseState)
			taskInfoList.at(taskInfoListId)->state = stopState;
		if ( taskInfoList.at(taskInfoListId)->state == runningState)
		{
			for (int taskThreadListId=0;taskThreadListId<taskInfoList.at(taskInfoListId)->taskThreadList.size();taskThreadListId++)
			{
				taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->threadTaskInfomation.httpRequestAborted = true;
				taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->setThreadStopped();
			}
			taskInfoList.at(taskInfoListId)->controlState = stopState;
			deleteTaskList.append(taskInfoListId);
		}
	}

	if (deleteTaskTimer == 0)
	{
		deleteTaskTimer = new QTimer(this);
		connect(deleteTaskTimer, SIGNAL(timeout()), this, SLOT(deleteTaskDownloadThread()));
		deleteTaskTimer->start(50);
	}
}
void MainWindow::deleteTask()
{
	int taskInfoListId = getCurrentTaskInfoListId();
	if (taskInfoListId == -1) return;
	DeleteTaskDialog *deleteTaskDialog = new DeleteTaskDialog(this);

	if ( deleteTaskDialog->exec() == QDialog::Accepted )
	{
		bool deleteUncompletedBool = deleteTaskDialog->deleteUncompletedCheckBox->isChecked();
		bool deleteDownloadedBool = deleteTaskDialog->deleteDownloadedCheckBox->isChecked();

		if (deleteUncompletedBool==true && taskInfoList.at(taskInfoListId)->state!=finishedState)
		{
			QString fileName = QString("%1/%2").arg(taskInfoList.at(taskInfoListId)->saveTo).arg(taskInfoList.at(taskInfoListId)->rename);
			QFile *file = new QFile(fileName);
			file->remove();
			delete file;
			file = 0;
			deleteTaskAndTableWidget(taskInfoListId);
			return;
		}
		if (deleteDownloadedBool==true && taskInfoList.at(taskInfoListId)->state==finishedState)
		{
			QString fileName = QString("%1/%2").arg(taskInfoList.at(taskInfoListId)->saveTo).arg(taskInfoList.at(taskInfoListId)->rename);
			QFile *file = new QFile(fileName);
			file->remove();
			delete file;
			file = 0;
			deleteTaskAndTableWidget(taskInfoListId);
			return;
		}
		if (deleteUncompletedBool==true && deleteDownloadedBool==true)
		{
			QString fileName = QString("%1/%2").arg(taskInfoList.at(taskInfoListId)->saveTo).arg(taskInfoList.at(taskInfoListId)->rename);
			QFile *file = new QFile(fileName);
			file->remove();
			delete file;
			file = 0;
			deleteTaskAndTableWidget(taskInfoListId);
			return;
		}
		deleteTaskAndTableWidget(taskInfoListId);
	}
}
void MainWindow::deleteTaskAndTableWidget(int taskInfoListId)
{
	QString taskFileName = QString("%1/%2.task").arg(taskInfoList.at(taskInfoListId)->saveTo).arg(taskInfoList.at(taskInfoListId)->rename);
	QFile *file = new QFile(taskFileName);
	file->remove();
	delete file;
	file = 0;
	int row = tableWidget->currentRow();
	tableWidget->removeRow(row);
	TaskInfomation *tempTaskInfo=taskInfoList.at(taskInfoListId);
	taskInfoList.removeAt(taskInfoListId);
	for (int notDownloadListId=0;notDownloadListId<tempTaskInfo->notDownloadList.size();notDownloadListId++)
	{
		NotDownload *tempNotDownload = tempTaskInfo->notDownloadList.at(0);
		tempTaskInfo->notDownloadList.removeAt(0);
		delete tempNotDownload;
	}
	delete tempTaskInfo;
	for (int i=taskInfoListId;i<taskInfoList.size();i++)
	{
		for (int taskThreadListId=0;taskThreadListId<taskInfoList.at(i)->taskThreadList.size();taskThreadListId++)
		{
			taskInfoList.at(i)->taskThreadList.at(taskThreadListId)->threadTaskInfomation.taskInfoListId = i;
		}
	}
	saveAllTask();
}
void MainWindow::redownloadTask()
{
	int taskInfoListId = getCurrentTaskInfoListId();
	if (taskInfoListId == -1) return;

	taskInfoList.at(taskInfoListId)->size = 0;
	taskInfoList.at(taskInfoListId)->state = stopState;
	while (!taskInfoList.at(taskInfoListId)->notDownloadList.isEmpty())
	{
		NotDownload *notDownload=taskInfoList.at(taskInfoListId)->notDownloadList.at(0);
		taskInfoList.at(taskInfoListId)->notDownloadList.removeAt(0);
		delete notDownload;
	}

	QString taskName = QString("%1/%2.task").arg(taskInfoList.at(taskInfoListId)->saveTo).arg(taskInfoList.at(taskInfoListId)->rename);
	QSettings settings(taskName, QSettings::IniFormat);

	settings.beginGroup("file");
	settings.setValue("size", taskInfoList.at(taskInfoListId)->size);
	settings.setValue("state", taskInfoList.at(taskInfoListId)->state);
	settings.endGroup();

	settings.beginGroup("notDownloadsList");
	settings.remove("");
	settings.endGroup();

	runTask();
}
void MainWindow::modifyTask()
{
	int taskInfoListId = getCurrentTaskInfoListId();
	if (taskInfoListId == -1) return;
	if (taskInfoList.at(taskInfoListId)->state==runningState || taskInfoList.at(taskInfoListId)->state==pauseState || taskInfoList.at(taskInfoListId)->controlState!=noState)
	{
		QMessageBox::information(this, tr("Information"), tr("Please stop task."));
		return;
	}
	ModifyTaskDialog *modifyTaskDialog = new ModifyTaskDialog(taskInfoListId, this);
	modifyTaskDialog->setWindowTitle(tr("Modify task"));

	if ( modifyTaskDialog->exec() == QDialog::Accepted )
	{
		taskInfoList.at(taskInfoListId)->taskInfoListId = taskInfoList.size();
		taskInfoList.at(taskInfoListId)->url.setUrl(modifyTaskDialog->urlLineEdit->text());
		taskInfoList.at(taskInfoListId)->referrer = modifyTaskDialog->referrerLineEdit->text();
		taskInfoList.at(taskInfoListId)->saveTo = modifyTaskDialog->saveToLineEdit->text();
		taskInfoList.at(taskInfoListId)->rename = modifyTaskDialog->renameLineEdit->text();
		taskInfoList.at(taskInfoListId)->splite = modifyTaskDialog->splitSpinBox->value();
		taskInfoList.at(taskInfoListId)->user = modifyTaskDialog->userLineEdit->text();
		taskInfoList.at(taskInfoListId)->password = modifyTaskDialog->passwordLineEdit->text();

		bool startImmediately;
		if ( modifyTaskDialog->immediatelyRadioButton->isChecked() )
			startImmediately = true;
		else
			startImmediately = false;

		taskInfoList.at(taskInfoListId)->comment = modifyTaskDialog->commentTextEdit->toPlainText();

		QList<QString> urlsList;
		for (int row=0;row<modifyTaskDialog->ulrsTableWidget->rowCount();row++)
		{
			QTableWidgetItem *item = modifyTaskDialog->ulrsTableWidget->item(row,1);
			QString urlString = item->text();
			urlsList<<urlString;
		}
		taskInfoList.at(taskInfoListId)->urlsList = urlsList;

		qint64 taskInfoListIdNotCompleted = 0;
		for (int notDownloadListId=0;notDownloadListId<taskInfoList.at(taskInfoListId)->notDownloadList.size();notDownloadListId++)
		{
			taskInfoListIdNotCompleted += taskInfoList.at(taskInfoListId)->notDownloadList.at(notDownloadListId)->endPosition - taskInfoList.at(taskInfoListId)->notDownloadList.at(notDownloadListId)->startPosition;
		}
		taskInfoList.at(taskInfoListId)->completed = taskInfoList.at(taskInfoListId)->size - taskInfoListIdNotCompleted;

		tableWidget->setItem(taskInfoListId, fileNameHeader, new QTableWidgetItem(taskInfoList.at(taskInfoListId)->rename));
		tableWidget->setItem(taskInfoListId, sizeHeader, new QTableWidgetItem(QString("%1").arg(taskInfoList.at(taskInfoListId)->size)));
		tableWidget->setItem(taskInfoListId, compeletedHeader, new QTableWidgetItem(QString("%1").arg(taskInfoList.at(taskInfoListId)->completed)));
		tableWidget->setItem(taskInfoListId, urlHeader, new QTableWidgetItem(taskInfoList.at(taskInfoListId)->url.toString()));

		QSettings settings("NetFleet","task");
		settings.beginGroup(QString("task%1").arg(taskInfoListId));
		settings.setValue("url", taskInfoList.at(taskInfoListId)->url.toString());
		settings.setValue("referrer", taskInfoList.at(taskInfoListId)->referrer);
		settings.setValue("saveTo", taskInfoList.at(taskInfoListId)->saveTo);
		settings.setValue("rename", taskInfoList.at(taskInfoListId)->rename);

		settings.setValue("split", taskInfoList.at(taskInfoListId)->splite);
		settings.setValue("user", taskInfoList.at(taskInfoListId)->user);
		settings.setValue("password", taskInfoList.at(taskInfoListId)->password);

		settings.setValue("comment", taskInfoList.at(taskInfoListId)->comment);

		QList <QVariant> urlsListVariant;
		foreach( QString url, urlsList )
		urlsListVariant << url;
		settings.setValue("urls", urlsListVariant);
		settings.endGroup();

		QSettings settings2;
		settings2.beginGroup("savePath");
		settings2.setValue("lastPath", taskInfoList.at(taskInfoListId)->saveTo);
		settings2.endGroup();

		if (startImmediately) runTask(taskInfoListId);
	}
}
void MainWindow::viewDropWindowTriggered()
{
	if (smallWindow->isVisible())
	{
		smallWindow->setVisible(false);
		viewDropWindowAct->setChecked(false);
	}
	else
	{
		smallWindow->setVisible(true);
		viewDropWindowAct->setChecked(true);
	}
}
void MainWindow::viewSpeedWidgetDockTriggered()
{
	if (speedWidgetDock->isVisible())
	{
		speedWidgetDock->setVisible(false);
		viewSpeedWidgetDockAct->setChecked(false);
	}
	else
	{
		speedWidgetDock->setVisible(true);
		viewSpeedWidgetDockAct->setChecked(true);
	}
}
void MainWindow::viewTaskToolBarTriggered()
{
	if (taskToolBar->isVisible())
	{
		taskToolBar->setVisible(false);
		viewTaskToolBarAct->setChecked(false);
	}
	else
	{
		taskToolBar->setVisible(true);
		viewTaskToolBarAct->setChecked(true);
	}
}
void MainWindow::monitorClipboardTriggered()
{
	if (taskPreferences.monitorClipboardBool)
	{
		taskPreferences.monitorClipboardBool = false;
		monitorClipboardAct->setChecked(false);
	}
	else
	{
		taskPreferences.monitorClipboardBool = true;
		monitorClipboardAct->setChecked(true);
	}

	QSettings settings;
	settings.beginGroup("monitor");
	settings.setValue("monitorClipboard", taskPreferences.monitorClipboardBool);
	settings.setValue("monitorTypes", taskPreferences.monitorTypesString);
	settings.endGroup();
}
void MainWindow::preferencesTriggered()
{
	showApplicationNormal();

	PreferencesDialog *preferencesDialog = new PreferencesDialog(this);

	if ( preferencesDialog->exec() == QDialog::Accepted )
	{
		taskPreferences.maxRunningTasksInt = preferencesDialog->maxRunningTasksSpinBox->value();
		taskPreferences.splitInt = preferencesDialog->splitSpinBox->value();
		taskPreferences.minimumSplitSizeInt64 = preferencesDialog->minimumSplitSizeSpinBox->value();

		taskPreferences.useLastPathBool = preferencesDialog->useLastPathRadioButton->isChecked();

		taskPreferences.useDefaultPathBool = preferencesDialog->useDefaultPathRadioButton->isChecked();
		taskPreferences.saveToString = preferencesDialog->saveToLineEdit->text();

		taskPreferences.connectTimeOutInt = preferencesDialog->connectTimeOutSpinBox->value();
		if (preferencesDialog->alwaysRetryCheckBox->isChecked())
			taskPreferences.maxRetryNumberInt = -1;
		else
			taskPreferences.maxRetryNumberInt = preferencesDialog->maxRetryNumberSpinBox->value();
		taskPreferences.retryDelayInt = preferencesDialog->retryDelaySpinBox->value();

		taskPreferences.maxSpeedGraph = preferencesDialog->maxSpeedSpinBox->value();
		taskPreferences.maxHeightGraph = preferencesDialog->maxHeightSpinBox->value();
		taskPreferences.pixelWidthGraph = preferencesDialog->pixelWidthSpinBox->value();

		taskPreferences.blockSizeGraph = preferencesDialog->blockSizeSpinBox->value();

		taskPreferences.monitorClipboardBool = preferencesDialog->monitorClipboardCheckBox->isChecked();
		monitorClipboardAct->setChecked(taskPreferences.monitorClipboardBool);
		taskPreferences.monitorTypesString = preferencesDialog->monitorTypesTextEdit->toPlainText();

		QList <QString> proxysList;
		for (int row=0;row<preferencesDialog->proxyTableWidget->rowCount();row++)
		{
			proxysList << preferencesDialog->proxyTableWidget->item(row, 0)->text();
			QString hostString = newProxyDialog->hostLineEdit->text();
			int portInt = newProxyDialog->portSpinBox->value();
			bool httpBool = newProxyDialog->httpRadioButton->isChecked();
			bool socks4Bool = newProxyDialog->socks4RadioButton->isChecked();
			bool socks5Bool = newProxyDialog->socks5RadioButton->isChecked();
			QString userString = newProxyDialog->userLineEdit->text();
			QString passwordString = newProxyDialog->passwordLineEdit->text();
		}


		QSettings settings;
		settings.beginGroup("task");
		settings.setValue("maxRunningTasks", taskPreferences.maxRunningTasksInt);
		settings.setValue("split", taskPreferences.splitInt);
		settings.setValue("minimumSplitSize", taskPreferences.minimumSplitSizeInt64);
		settings.endGroup();

		settings.beginGroup("savePath");
		settings.setValue("useLastPath", taskPreferences.useLastPathBool);
		settings.setValue("useDefaultPath", taskPreferences.useDefaultPathBool);
		settings.setValue("defaultPath", taskPreferences.saveToString);
		settings.endGroup();

		settings.beginGroup("connection");
		settings.setValue("connectTimeOut", taskPreferences.connectTimeOutInt);
		settings.setValue("maxRetryNumber", taskPreferences.maxRetryNumberInt);
		settings.setValue("retryDelay", taskPreferences.retryDelayInt);
		settings.endGroup();

		settings.beginGroup("speedGraph");
		settings.setValue("maxSpeed", taskPreferences.maxSpeedGraph);
		settings.setValue("maxHeight", taskPreferences.maxHeightGraph);
		settings.setValue("pixelWidth", taskPreferences.pixelWidthGraph);
		settings.endGroup();

		settings.beginGroup("blocksGraph");
		settings.setValue("blockSize", taskPreferences.blockSizeGraph);
		settings.endGroup();

		settings.beginGroup("monitor");
		settings.setValue("monitorClipboard", taskPreferences.monitorClipboardBool);
		settings.setValue("monitorTypes", taskPreferences.monitorTypesString);
		settings.endGroup();
	}
	delete preferencesDialog;
	preferencesDialog = 0;

	speedGraphWidget->setData(taskPreferences.maxSpeedGraph*1024, taskPreferences.maxHeightGraph, taskPreferences.pixelWidthGraph);
	smallWindow->setData(taskPreferences.maxSpeedGraph*1024, taskPreferences.pixelWidthGraph);
	taskGraphPainterWidget->setBlockSizeData( taskPreferences.blockSizeGraph * 1024);
}
void MainWindow::taskTableWidgetSelectionChanged()
{
	int currentTaskInfoListId = getCurrentTaskInfoListId();
	if (currentTaskInfoListId == -1) return;

	int n = threadInfomationTreeWidgetItem->childCount();
	while (n < taskInfoList.at(currentTaskInfoListId)->taskThreadList.size())
	{
		threadNumberTreeWidgetItem = new QTreeWidgetItem(threadInfomationTreeWidgetItem);
		threadNumberTreeWidgetItem->setText(0, QString("Thread %1").arg(n));
		n = threadInfomationTreeWidgetItem->childCount();
	}
	while (n > taskInfoList.at(currentTaskInfoListId)->taskThreadList.size())
	{
		threadNumberTreeWidgetItem = threadInfomationTreeWidgetItem->child(n-1);
		threadInfomationTreeWidgetItem->removeChild(threadNumberTreeWidgetItem);
		delete threadNumberTreeWidgetItem;
		threadNumberTreeWidgetItem = 0;
		n = threadInfomationTreeWidgetItem->childCount();
	}

	threadNumberTreeWidget->expandAll();
	threadNumberSelectionChanged();
	showActionState();
}
void MainWindow::taskTableWidgetDoubleClicked(int row, int column)
{
	int taskInfoListId = getCurrentTaskInfoListId();
	if (taskInfoListId == -1) return;

	if (taskInfoList.at(taskInfoListId)->state == finishedState)
	{
		QString taskName = QString("%1/%2").arg(taskInfoList.at(taskInfoListId)->saveTo).arg(taskInfoList.at(taskInfoListId)->rename);
		QProcess *myProcess = new QProcess(0);
		myProcess->start(taskName);
	}
}
void MainWindow::threadNumberSelectionChanged()
{
	int taskInfoListId = getCurrentTaskInfoListId();
	if (taskInfoListId == -1) return;
	int taskThreadListId = getCurrentTaskThreadListId();
	if (taskThreadListId == -1 || taskInfoList.at(taskInfoListId)->taskThreadList.isEmpty())
	{
		showGraphWidget(true);
		taskGraphWidget->show();
		threadsDetailTableWidget->hide();

		return;
	}

	threadsDetailTableWidgetClear();
	threadsDetailTableWidget->show();
	taskGraphWidget->hide();

	for (int i=0;i<taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->threadTaskInfomation.stateDetailList.size();i++)
	{
		int row = threadsDetailTableWidget->rowCount();
		threadsDetailTableWidget->insertRow(row);
		QIcon stateIcon = taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->threadTaskInfomation.stateDetailList.at(i);
		QString timeString = taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->threadTaskInfomation.timeDetailList.at(i);
		QString messageString = taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->threadTaskInfomation.messageDetailList.at(i);
		threadsDetailTableWidget->setItem(row, 0, new QTableWidgetItem(stateIcon, timeString));
		threadsDetailTableWidget->setItem(row, 1, new QTableWidgetItem(messageString));

		QFontMetrics fm = threadsDetailTableWidget->fontMetrics();
		int pixelsHigh = fm.height();
		threadsDetailTableWidget->setRowHeight(row,pixelsHigh);
	}
	threadsDetailTableWidget->resizeColumnToContents(0);
	threadsDetailTableWidget->setColumnWidth(1, threadGraphDetailWidget->width()-threadsDetailTableWidget->columnWidth(0)-38);
	threadsDetailTableWidget->scrollToBottom();
}
void MainWindow::closeEvent(QCloseEvent *event)
{
	if (trayIcon->isVisible())
	{
		hide();
		event->ignore();
	}
}
void MainWindow::changeEvent(QEvent *event)
{
	if (event->type() == QEvent::WindowStateChange)
	{
		if (isMinimized())
		{
			event->ignore();
			hide();
			return;
		}
	}
}
#ifdef WIN32
bool MainWindow::winEvent( MSG *m, long *result )
{
	switch ( m->message )
	{
	case WM_SYSCOMMAND:
	{
		if ( m->wParam == SC_MINIMIZE )
		{
//			ShowWindow(winId(), SW_HIDE);
			hide();
			return true;
		}
	}
	break;
	default:
		break;
	}
	return QWidget::winEvent(m, result);
}
#endif
void MainWindow::saveSettings()
{
	QSettings settings;
	settings.beginGroup("MainWindow");
	settings.setValue("pos", pos());
	settings.setValue("size", size());
	settings.setValue("MainWindowGeometry", saveGeometry());
	settings.setValue("MainWindowState", saveState());
	settings.setValue("tableWidgetState", tableWidget->saveGeometry());
	QList <int> infoDetailSplitterList = infoDetailSplitter->sizes();
	QList <QVariant> infoDetailSplitterListVariant;
	foreach( int i, infoDetailSplitterList )
	infoDetailSplitterListVariant << i;
	settings.setValue("infoDetailSplitterSizes", infoDetailSplitterListVariant);
	bool showSpeedWidgetDockBool = true;
	if (speedWidgetDock->isVisible())
		showSpeedWidgetDockBool = true;
	else
		showSpeedWidgetDockBool = false;
	settings.setValue("showSpeedWidgetDock", showSpeedWidgetDockBool);
	bool showTaskToolBarBool = true;
	if (taskToolBar->isVisible())
		showTaskToolBarBool = true;
	else
		showTaskToolBarBool = false;
	settings.setValue("showTaskToolBar", showTaskToolBarBool);
	settings.endGroup();

	settings.beginGroup("SmallWindow");
	settings.setValue("pos", smallWindow->pos());
	bool showSmallWindowBool = true;
	if (smallWindow->isVisible())
		showSmallWindowBool = true;
	else
		showSmallWindowBool = false;
	settings.setValue("show", showSmallWindowBool);
	settings.endGroup();
}
void MainWindow::readSettings()
{
	QSettings settings;
	settings.beginGroup("MainWindow");
	QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
	QSize size = settings.value("size", QSize(640, 480)).toSize();
	QByteArray state = settings.value("state", QByteArray()).toByteArray();
	resize(size);
	move(pos);
	restoreGeometry(settings.value("MainWindowGeometry").toByteArray());
	restoreState(settings.value("MainWindowState").toByteArray());
	tableWidget->restoreGeometry(settings.value("tableWidgetState").toByteArray());
	QList <int> infoDetailSplitterList;
	QList <QVariant> infoDetailSplitterListVariant;
	QList <QVariant> infoDetailSplitterListVariantDefault;
	infoDetailSplitterListVariantDefault<<130<<600;
	infoDetailSplitterListVariant = settings.value("infoDetailSplitterSizes",infoDetailSplitterListVariantDefault).toList();
	foreach( QVariant i, infoDetailSplitterListVariant )
	infoDetailSplitterList << i.toInt();
	infoDetailSplitter->setSizes(infoDetailSplitterList);
	bool showSpeedWidgetDockBool = settings.value("showSpeedWidgetDock", true).toBool();
	if (showSpeedWidgetDockBool)
	{
		speedWidgetDock->setVisible(true);
		viewSpeedWidgetDockAct->setChecked(true);
	}
	else
	{
		speedWidgetDock->setVisible(false);
		viewSpeedWidgetDockAct->setChecked(false);
	}
	bool showTaskToolBarBool = settings.value("showTaskToolBar", true).toBool();
	if (showTaskToolBarBool)
	{
		taskToolBar->setVisible(true);
		viewTaskToolBarAct->setChecked(true);
	}
	else
	{
		taskToolBar->setVisible(false);
		viewTaskToolBarAct->setChecked(false);
	}
	settings.endGroup();

	settings.beginGroup("SmallWindow");
	pos = settings.value("pos", QPoint(0, 0)).toPoint();
	smallWindow->move(pos);
	bool showSmallWindowBool = settings.value("show", true).toBool();
	if (showSmallWindowBool)
	{
		smallWindow->setVisible(true);
		viewDropWindowAct->setChecked(true);
	}
	else
	{
		smallWindow->setVisible(false);
		viewDropWindowAct->setChecked(false);
	}
	settings.endGroup();

	settings.beginGroup("task");
	taskPreferences.maxRunningTasksInt = settings.value("maxRunningTasks", 5).toInt();
	taskPreferences.splitInt = settings.value("split", 5).toInt();
	taskPreferences.minimumSplitSizeInt64 = settings.value("minimumSplitSize", 300).toInt();
	settings.endGroup();

	settings.beginGroup("savePath");
	taskPreferences.useLastPathBool = settings.value("useLastPath", true).toBool();
	taskPreferences.useDefaultPathBool = settings.value("useDefaultPath", false).toBool();
	taskPreferences.saveToString = settings.value("defaultPath").toString();
	settings.endGroup();

	settings.beginGroup("connection");
	taskPreferences.connectTimeOutInt = settings.value("connectTimeOut", 30).toInt();
	taskPreferences.maxRetryNumberInt = settings.value("maxRetryNumber", 99).toInt();
	taskPreferences.retryDelayInt = settings.value("retryDelay", 5).toInt();
	settings.endGroup();

	settings.beginGroup("speedGraph");
	taskPreferences.maxSpeedGraph = settings.value("maxSpeed", 250).toLongLong();
	taskPreferences.maxHeightGraph = settings.value("maxHeight", 60).toInt();
	taskPreferences.pixelWidthGraph = settings.value("pixelWidth", 2).toInt();
	settings.endGroup();

	settings.beginGroup("blocksGraph");
	taskPreferences.blockSizeGraph = settings.value("blockSize", 50).toLongLong();
	settings.endGroup();

	settings.beginGroup("monitor");
	taskPreferences.monitorClipboardBool = settings.value("monitorClipboard", true).toBool();
	monitorClipboardAct->setChecked(taskPreferences.monitorClipboardBool);
	QString defaultMonitorTypes = ".zip;.rar;.gz;.z;.bz2;.tar;.arj;.lzh;.exe;.bin;.mp3;.mp4";
	taskPreferences.monitorTypesString = settings.value("monitorTypes", defaultMonitorTypes).toString();
	settings.endGroup();

	speedGraphWidget->setData(taskPreferences.maxSpeedGraph*1024, taskPreferences.maxHeightGraph, taskPreferences.pixelWidthGraph);
	smallWindow->setData(taskPreferences.maxSpeedGraph*1024, taskPreferences.pixelWidthGraph);
}
int MainWindow::getCurrentTaskInfoListId()
{
	int row = tableWidget->currentRow ();
	QTableWidgetItem *currentTableWidgetItem;
	currentTableWidgetItem = tableWidget->item(row, urlHeader);
	int taskInfoListId=-1;
	for (int i=0;i<taskInfoList.size();i++)
	{
		QTableWidgetItem *tableWidgetItem;
		tableWidgetItem = tableWidget->item(i, urlHeader);
		if (currentTableWidgetItem == tableWidgetItem)
		{
			taskInfoListId = i;
			break;
		}
	}
	return taskInfoListId;
}
int MainWindow::getCurrentTaskThreadListId()
{
	QList<QTreeWidgetItem *> selectedItems;
	selectedItems = threadNumberTreeWidget->selectedItems();
	if (selectedItems.isEmpty()) return -1;
	QTreeWidgetItem *currentTreeWidgetItem;
	currentTreeWidgetItem = selectedItems.at(0);

	QString taskThreadListIdString = currentTreeWidgetItem->text(0);
	if (taskThreadListIdString == "Infomation") return -1;

	if (taskThreadListIdString.size()<6) return -1;
	taskThreadListIdString = taskThreadListIdString.mid(6);
	int taskThreadListId = taskThreadListIdString.toInt();

	return taskThreadListId;
}
void MainWindow::showTaskState(int taskInfoListId)
{
	QTableWidgetItem *stateTableWidgetItem;
	stateTableWidgetItem = tableWidget->item(taskInfoListId, stateHeader);
	tableWidget->takeItem(taskInfoListId, stateHeader);
	delete stateTableWidgetItem;

	//set finished state
	if (taskInfoList.at(taskInfoListId)->state==runningState && taskInfoList.at(taskInfoListId)->controlState==noState && taskInfoList.at(taskInfoListId)->completed==taskInfoList.at(taskInfoListId)->size && taskInfoList.at(taskInfoListId)->completed>0)
	{
#if defined(DEBUG)
		qDebug()<<qPrintable(QString("set finished taskid%1 controlState=finishedState").arg(taskInfoListId));
#endif
		showSmallWindow();
		taskInfoList.at(taskInfoListId)->controlState = finishedState;
		deleteTaskList.append(taskInfoListId);

#if defined(DEBUG)
		qDebug("set finished state********************");
		for (int j=0;j<taskInfoList.at(taskInfoListId)->notDownloadList.size();j++)
		{
			qint64 startPosition = taskInfoList.at(taskInfoListId)->notDownloadList.at(j)->startPosition;
			qint64 endPosition = taskInfoList.at(taskInfoListId)->notDownloadList.at(j)->endPosition;
			int listId = taskInfoList.at(taskInfoListId)->notDownloadList.at(j)->taskThreadListId;
			qDebug()<<qPrintable(QString("%1:%2---%3").arg(listId).arg(startPosition).arg(endPosition));
		}
#endif

		if (deleteTaskTimer == 0)
		{
#if defined(DEBUG)
			qDebug()<<"start deleteTaskTimer";
#endif
			deleteTaskTimer = new QTimer(this);
			connect(deleteTaskTimer, SIGNAL(timeout()), this, SLOT(deleteTaskDownloadThread()));
			deleteTaskTimer->start(50);
		}
	}

	switch (taskInfoList.at(taskInfoListId)->state)
	{
	case stopState:
		tableWidget->setItem(taskInfoListId, stateHeader, new QTableWidgetItem(QIcon(":/images/stopstate.png"),""));
		break;
	case runningState:
		tableWidget->setItem(taskInfoListId, stateHeader, new QTableWidgetItem(QIcon(":/images/runningstate.png"),""));
		break;
	case finishedState:
		tableWidget->setItem(taskInfoListId, stateHeader, new QTableWidgetItem(QIcon(":/images/finishedstate.png"),""));
		break;
	case pauseState:
		tableWidget->setItem(taskInfoListId, stateHeader, new QTableWidgetItem(QIcon(":/images/pausestate.png"),""));
		break;
	case errorState:
		tableWidget->setItem(taskInfoListId, stateHeader, new QTableWidgetItem(QIcon(":/images/errorstate.png"),""));
		break;
	}
}
void MainWindow::deleteTaskDownloadThread()
{
	if (!deleteTaskList.isEmpty())
	{
		int taskStoppedCount = 0;
		int taskInfoListId = deleteTaskList.at(0);
		for (int taskThreadListId=0;taskThreadListId<taskInfoList.at(taskInfoListId)->taskThreadList.size();taskThreadListId++)
		{
			if (taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->threadStopped==true && taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->isRunning()==false)
			{
				taskStoppedCount ++;
			}
		}
		if (taskStoppedCount==taskInfoList.at(taskInfoListId)->taskThreadList.size())
		{
			showGraphWidget(false);
			while (!taskInfoList.at(taskInfoListId)->taskThreadList.isEmpty())
			{
				disconnect(taskInfoList.at(taskInfoListId)->taskThreadList.at(0), SIGNAL(returnThreadMessages(int,int,int, QString)), this, SLOT(showThreadMessages(int,int,int, QString)));
				disconnect(taskInfoList.at(taskInfoListId)->taskThreadList.at(0), SIGNAL(fileDownloadFinished(int,int)), this, SLOT(getFileDownloadFinished(int,int)));
				disconnect(taskInfoList.at(taskInfoListId)->taskThreadList.at(0), SIGNAL(returnFileSize(int,qint64)), this, SLOT(getFileSize(int,qint64)));

				DownloadThread *downloadThread = taskInfoList.at(taskInfoListId)->taskThreadList.at(0);
				taskInfoList.at(taskInfoListId)->taskThreadList.removeAt(0);
				downloadThread->wait();
				delete downloadThread;
#if defined(DEBUG)
				qDebug()<<qPrintable(QString("threadCount:%1").arg(taskInfoList.at(taskInfoListId)->taskThreadList.size()));
#endif
			}
			for (int i=0;i<taskInfoList.at(taskInfoListId)->notDownloadList.size();i++)
			{
				taskInfoList.at(taskInfoListId)->notDownloadList.at(i)->taskThreadListId = -1;
			}
			taskTableWidgetSelectionChanged();
			deleteTaskList.removeAt(0);
#if defined(DEBUG)
			qDebug()<<qPrintable(QString("set taskid%1 state=%2").arg(taskInfoListId).arg(taskInfoList.at(taskInfoListId)->controlState));
#endif
			taskInfoList.at(taskInfoListId)->state = taskInfoList.at(taskInfoListId)->controlState;
#if defined(DEBUG)
			qDebug()<<"set controlState = noState";
#endif
			taskInfoList.at(taskInfoListId)->controlState = noState;
			saveTaskState(taskInfoListId);
		}
	}
	else
	{
		deleteTaskTimer->stop();
		delete deleteTaskTimer;
		deleteTaskTimer = 0;
	}

}
void MainWindow::startFinishedThread(int taskInfoListId, int taskThreadListId)
{
	for (int notDownloadListId=0;notDownloadListId<taskInfoList.at(taskInfoListId)->notDownloadList.size();notDownloadListId++)
	{
		if (taskInfoList.at(taskInfoListId)->notDownloadList.at(notDownloadListId)->taskThreadListId == -1)
		{
#if defined(DEBUG)
			qDebug("startFinishedThread********************");
			for (int j=0;j<taskInfoList.at(taskInfoListId)->notDownloadList.size();j++)
			{
				qint64 startPosition = taskInfoList.at(taskInfoListId)->notDownloadList.at(j)->startPosition;
				qint64 endPosition = taskInfoList.at(taskInfoListId)->notDownloadList.at(j)->endPosition;
				int listId = taskInfoList.at(taskInfoListId)->notDownloadList.at(j)->taskThreadListId;
				qDebug()<<qPrintable(QString("%1:%2---%3").arg(listId).arg(startPosition).arg(endPosition));
			}
#endif

			taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->threadTaskInfomation.newReceivedList.clear();
			threadNewReceivedListAppend(taskInfoListId, taskThreadListId, notDownloadListId);

			startTaskThread(taskInfoListId, taskThreadListId, notDownloadListId);
#if defined(DEBUG)
			qDebug("startedFinishedThread********************");
			for (int j=0;j<taskInfoList.at(taskInfoListId)->notDownloadList.size();j++)
			{
				qint64 startPosition = taskInfoList.at(taskInfoListId)->notDownloadList.at(j)->startPosition;
				qint64 endPosition = taskInfoList.at(taskInfoListId)->notDownloadList.at(j)->endPosition;
				int listId = taskInfoList.at(taskInfoListId)->notDownloadList.at(j)->taskThreadListId;
				qDebug()<<qPrintable(QString("%1:%2---%3").arg(listId).arg(startPosition).arg(endPosition));
			}
#endif
			return;
		}

	}
#if defined(DEBUG)
	qDebug("before********************");
	for (int j=0;j<taskInfoList.at(taskInfoListId)->notDownloadList.size();j++)
	{
		qint64 startPosition = taskInfoList.at(taskInfoListId)->notDownloadList.at(j)->startPosition;
		qint64 endPosition = taskInfoList.at(taskInfoListId)->notDownloadList.at(j)->endPosition;
		int listId = taskInfoList.at(taskInfoListId)->notDownloadList.at(j)->taskThreadListId;
		qDebug()<<qPrintable(QString("%1:%2---%3").arg(listId).arg(startPosition).arg(endPosition));
	}
#endif
	if (newNotDownload(taskInfoListId) == false) return;
#if defined(DEBUG)
	qDebug()<<qPrintable(QString("start finished thread%1").arg(taskThreadListId));
#endif
	taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->threadTaskInfomation.newReceivedList.clear();
	threadNewReceivedListAppend(taskInfoListId, taskThreadListId, taskInfoList.at(taskInfoListId)->notDownloadList.size()-1);

	startTaskThread(taskInfoListId, taskThreadListId, taskInfoList.at(taskInfoListId)->notDownloadList.size()-1);
#if defined(DEBUG)
	qDebug("after********************");
	for (int j=0;j<taskInfoList.at(taskInfoListId)->notDownloadList.size();j++)
	{
		qint64 startPosition = taskInfoList.at(taskInfoListId)->notDownloadList.at(j)->startPosition;
		qint64 endPosition = taskInfoList.at(taskInfoListId)->notDownloadList.at(j)->endPosition;
		int listId = taskInfoList.at(taskInfoListId)->notDownloadList.at(j)->taskThreadListId;
		qDebug()<<qPrintable(QString("%1:%2---%3").arg(listId).arg(startPosition).arg(endPosition));
	}
#endif
}
void MainWindow::threadsDetailTableWidgetClear()
{
	if (threadsDetailTableWidget->rowCount() > 0)
	{
		for (int row=threadsDetailTableWidget->rowCount()-1;row>=0;row--)
		{
			for (int column=threadsDetailTableWidget->columnCount()-1;column>=0;column--)
			{
				QTableWidgetItem *tempItem = threadsDetailTableWidget->item(row, column);
				threadsDetailTableWidget->takeItem (row, column );
				delete tempItem;
			}
		}
		threadsDetailTableWidget->setRowCount(0);
	}
}
bool MainWindow::newNotDownload(int taskInfoListId)
{
	if (taskInfoList.at(taskInfoListId)->notDownloadList.isEmpty() && taskInfoList.at(taskInfoListId)->state==finishedState)
		return false;
	qint64 newThreadStartPosition = 0;
	qint64 newThreadEndPosition = 0;
	qint64 notDownloadBytes = 0;
	if (taskInfoList.at(taskInfoListId)->notDownloadList.isEmpty())
	{
#if defined(DEBUG)
		qDebug("newThreadStartPosition::0");
#endif
		notDownload = new NotDownload;
		notDownload->taskThreadListId = -1;
		notDownload->startPosition = 0;
		notDownload->endPosition = 0;
		taskInfoList.at(taskInfoListId)->notDownloadList.append(notDownload);
		return true;
	}
	qint64 startPosition = 0;
	qint64 endPosition = 0;
	qint64 tempId = 0;
	for (int i=0;i<taskInfoList.at(taskInfoListId)->notDownloadList.size();i++)
	{
		startPosition = taskInfoList.at(taskInfoListId)->notDownloadList.at(i)->startPosition;
		endPosition = taskInfoList.at(taskInfoListId)->notDownloadList.at(i)->endPosition;
		if (endPosition - startPosition > notDownloadBytes)
		{
			notDownloadBytes = endPosition - startPosition;
			newThreadStartPosition = startPosition + notDownloadBytes/2;
			newThreadEndPosition = endPosition;
			tempId = i;
		}
	}
	if (notDownloadBytes >= taskPreferences.minimumSplitSizeInt64*1024)
	{
		notDownload = new NotDownload;
		taskInfoList.at(taskInfoListId)->notDownloadList.append(notDownload);
		taskInfoList.at(taskInfoListId)->notDownloadList.at(taskInfoList.at(taskInfoListId)->notDownloadList.size()-1)->taskThreadListId = -1;
		taskInfoList.at(taskInfoListId)->notDownloadList.at(taskInfoList.at(taskInfoListId)->notDownloadList.size()-1)->startPosition = newThreadStartPosition;
		taskInfoList.at(taskInfoListId)->notDownloadList.at(taskInfoList.at(taskInfoListId)->notDownloadList.size()-1)->endPosition = newThreadEndPosition;
		taskInfoList.at(taskInfoListId)->notDownloadList.at(tempId)->endPosition = newThreadStartPosition;
#if defined(DEBUG)
		qDebug()<<qPrintable(QString("newThreadStartPosition:%1").arg(newThreadStartPosition));
#endif

		return true;
	}
	return false;
}
void MainWindow::saveAllTask()
{
	QSettings settings("NetFleet","task");
	settings.clear();
	for (int taskInfoListId=0;taskInfoListId<taskInfoList.size();taskInfoListId++)
	{
		settings.beginGroup(QString("task%1").arg(taskInfoListId));
		settings.setValue("url", taskInfoList.at(taskInfoListId)->url.toString());
		settings.setValue("saveTo", taskInfoList.at(taskInfoListId)->saveTo);
		settings.setValue("rename", taskInfoList.at(taskInfoListId)->rename);
		settings.setValue("comment", taskInfoList.at(taskInfoListId)->comment);
		settings.setValue("split", QString::number(taskInfoList.at(taskInfoListId)->splite));
		settings.setValue("user", taskInfoList.at(taskInfoListId)->user);
		settings.setValue("password", taskInfoList.at(taskInfoListId)->password);
		settings.endGroup();
	}
}
void MainWindow::saveTaskState(int taskInfoListId)
{
	QString taskName = QString("%1/%2.task").arg(taskInfoList.at(taskInfoListId)->saveTo).arg(taskInfoList.at(taskInfoListId)->rename);
	QSettings settings(taskName, QSettings::IniFormat);

	settings.beginGroup("file");
	settings.setValue("size", taskInfoList.at(taskInfoListId)->size);
	settings.setValue("state", taskInfoList.at(taskInfoListId)->state);
	settings.endGroup();

	settings.beginGroup("notDownloadsList");
	settings.remove("");
	for (int notDownloadListId=0;notDownloadListId<taskInfoList.at(taskInfoListId)->notDownloadList.size();notDownloadListId++)
	{
		QString notDownloadListIdString = QString("notDownloadId%1").arg(notDownloadListId);
		QStringList notDownloadListIdValueString;
		notDownloadListIdValueString<<QString("%1").arg(taskInfoList.at(taskInfoListId)->notDownloadList.at(notDownloadListId)->startPosition)<<QString("%1").arg(taskInfoList.at(taskInfoListId)->notDownloadList.at(notDownloadListId)->endPosition);
		settings.setValue(notDownloadListIdString, notDownloadListIdValueString);
	}
	settings.endGroup();
}
void MainWindow::readTaskState(int taskInfoListId)
{
	QString taskName = QString("%1/%2.task").arg(taskInfoList.at(taskInfoListId)->saveTo).arg(taskInfoList.at(taskInfoListId)->rename);
	QSettings settings(taskName, QSettings::IniFormat);

	settings.beginGroup("file");
	taskInfoList.at(taskInfoListId)->size = settings.value("size", 0).toLongLong();
	taskInfoList.at(taskInfoListId)->state = settings.value("state", 0).toInt();
	if (taskInfoList.at(taskInfoListId)->state==pauseState || taskInfoList.at(taskInfoListId)->state==runningState)
		taskInfoList.at(taskInfoListId)->state = stopState;
	settings.endGroup();

	settings.beginGroup("notDownloadsList");
	int notDownloadListId=0;
	QStringList notDownloadListIdValueString;
	QStringList defaultValue;
	defaultValue<<"-1"<<"-1";
	taskInfoList.at(taskInfoListId)->notDownloadList.clear();
	while (true)
	{
		QString notDownloadListIdString = QString("notDownloadId%1").arg(notDownloadListId);
		notDownloadListIdValueString.clear();
		notDownloadListIdValueString<<settings.value(notDownloadListIdString, defaultValue).toStringList();
		if (notDownloadListIdValueString.at(0) == "-1") break;
		notDownload = new NotDownload;
		notDownload->taskThreadListId = -1;
		notDownload->startPosition = notDownloadListIdValueString.at(0).toLongLong();
		notDownload->endPosition = notDownloadListIdValueString.at(1).toLongLong();
		taskInfoList.at(taskInfoListId)->notDownloadList.append(notDownload);
		notDownloadListId++;
	}
	settings.endGroup();
}
void MainWindow::connectTaskThread(int taskInfoListId, int taskThreadListId)
{
	connect(taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId), SIGNAL(returnThreadMessages(int,int,int, QString)), this, SLOT(showThreadMessages(int,int,int, QString)));
	connect(taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId), SIGNAL(fileDownloadFinished(int,int)), this, SLOT(getFileDownloadFinished(int,int)));
	connect(taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId), SIGNAL(returnFileSize(int,qint64)), this, SLOT(getFileSize(int,qint64)));
}
void MainWindow::showGraphWidget(bool refreshAll, int taskInfoListId, int newThreadReceivedListId)
{
	int currentTaskInfoListId = getCurrentTaskInfoListId();
	if (currentTaskInfoListId==-1) return;
	if (taskInfoListId!=-1 && currentTaskInfoListId!=taskInfoListId) return;

	qint64 size = taskInfoList.at(currentTaskInfoListId)->size;
	taskGraphPainterWidget->setData( size, taskPreferences.blockSizeGraph*1024);
	QList <NotDownload *> notDownloadList = taskInfoList.at(currentTaskInfoListId)->notDownloadList;
	taskGraphPainterWidget->setNotDownloadListClear();
	for (int notDownloadListId=0;notDownloadListId<taskInfoList.at(currentTaskInfoListId)->notDownloadList.size();notDownloadListId++)
	{
		int taskThreadListId = taskInfoList.at(currentTaskInfoListId)->notDownloadList.at(notDownloadListId)->taskThreadListId;
		qint64 startPosition = taskInfoList.at(currentTaskInfoListId)->notDownloadList.at(notDownloadListId)->startPosition;
		qint64 endPosition = taskInfoList.at(currentTaskInfoListId)->notDownloadList.at(notDownloadListId)->endPosition;
		taskGraphPainterWidget->setNotDownloadList(taskThreadListId, startPosition, endPosition);
	}
	taskGraphPainterWidget->newReceivedListClear();
	for (int taskThreadListId=0;taskThreadListId<taskInfoList.at(currentTaskInfoListId)->taskThreadList.size();taskThreadListId++)
	{
		QList <qint64> newReceivedList;
		newReceivedList = taskInfoList.at(currentTaskInfoListId)->taskThreadList.at(taskThreadListId)->threadTaskInfomation.newReceivedList;
		if (taskInfoList.at(currentTaskInfoListId)->taskThreadList.at(taskThreadListId)->threadStopped==true)
			taskGraphPainterWidget->setNewReceived(-1, newReceivedList);
		else
			taskGraphPainterWidget->setNewReceived(taskThreadListId, newReceivedList);
	}
	if (newThreadReceivedListId != -1)
	{
		taskGraphPainterWidget->refreshThreadLastBlock(newThreadReceivedListId);
		return;
	}

	if (refreshAll == true)
	{
		taskGraphPainterWidget->refreshAll();
	}
	else
	{
		taskGraphPainterWidget->refreshPixmap();
	}
}
void MainWindow::threadNewReceivedListAppend(int taskInfoListId, int taskThreadListId, int notDownloadListId)
{
	qint64 startPosition = taskInfoList.at(taskInfoListId)->notDownloadList.at(notDownloadListId)->startPosition;
	taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->threadTaskInfomation.newReceivedList.append(startPosition);
	while (taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->threadTaskInfomation.newReceivedList.size() > 3)
		taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->threadTaskInfomation.newReceivedList.removeAt(0);
}
int MainWindow::getRunningTasksCount()
{
	int runningTasksInt = 0;
	for (int taskInfoListId=0;taskInfoListId<taskInfoList.size();taskInfoListId++)
	{
		if (taskInfoList.at(taskInfoListId)->state == runningState) runningTasksInt++;
	}

	return runningTasksInt;
}
void MainWindow::startTaskThread(int taskInfoListId, int taskThreadListId, int notDownloadListId, bool connectTaskThreadBool)
{
	taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->threadTaskInfomation.taskInfoListId = taskInfoListId;
	taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->threadTaskInfomation.taskThreadListId = taskThreadListId;
	taskInfoList.at(taskInfoListId)->notDownloadList.at(notDownloadListId)->taskThreadListId = taskThreadListId;
	if (taskInfoList.at(taskInfoListId)->urlsList.size()>taskThreadListId && taskInfoList.at(taskInfoListId)->urlsList.at(taskThreadListId)!="")
		taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->threadTaskInfomation.url = taskInfoList.at(taskInfoListId)->urlsList.at(taskThreadListId);
	else
		taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->threadTaskInfomation.url = taskInfoList.at(taskInfoListId)->url;
	taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->threadTaskInfomation.referrer = taskInfoList.at(taskInfoListId)->referrer;
	QString taskFileName = QString("%1/%2").arg(taskInfoList.at(taskInfoListId)->saveTo).arg(taskInfoList.at(taskInfoListId)->rename);
	taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->threadTaskInfomation.fileName = taskFileName;
	taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->threadTaskInfomation.size = taskInfoList.at(taskInfoListId)->size;
	taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->threadTaskInfomation.startPosition = taskInfoList.at(taskInfoListId)->notDownloadList.at(notDownloadListId)->startPosition;
	taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->threadTaskInfomation.completed = 0;
	taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->threadStopped = false;
	if (connectTaskThreadBool == true)
		connectTaskThread(taskInfoListId, taskThreadListId);
	taskInfoList.at(taskInfoListId)->taskThreadList.at(taskThreadListId)->start();
}
void MainWindow::showActionState()
{
	int taskInfoListId = getCurrentTaskInfoListId();
	if (taskInfoListId == -1) return;

	if (taskInfoList.at(taskInfoListId)->state == runningState)
	{
		runTaskAct->setEnabled(false);
		stopTaskAct->setEnabled(true);
		deleteTaskAct->setEnabled(false);
		redownloadTaskAct->setEnabled(false);
		modifyTaskAct->setEnabled(false);
	}
	else
	{
		runTaskAct->setEnabled(true);
		stopTaskAct->setEnabled(false);
		deleteTaskAct->setEnabled(true);
		redownloadTaskAct->setEnabled(true);
		modifyTaskAct->setEnabled(true);
	}
	if (taskInfoList.at(taskInfoListId)->state == finishedState)
	{
		runTaskAct->setEnabled(false);
		stopTaskAct->setEnabled(false);
		deleteTaskAct->setEnabled(true);
		redownloadTaskAct->setEnabled(true);
		modifyTaskAct->setEnabled(true);
	}
}
void MainWindow::createTaskTableWidgetContextMenu()
{
	tableWidget->addAction(addTaskAct);
	tableWidget->addAction(separatorAct);
	tableWidget->addAction(runTaskAct);
	tableWidget->addAction(stopTaskAct);
	tableWidget->addAction(deleteTaskAct);
	tableWidget->addAction(separatorAct2);
	tableWidget->addAction(modifyTaskAct);
	tableWidget->addAction(redownloadTaskAct);
	tableWidget->addAction(separatorAct3);
	tableWidget->addAction(runAllTaskAct);
	tableWidget->addAction(stopAllTaskAct);
	tableWidget->setContextMenuPolicy(Qt::ActionsContextMenu);
}
void MainWindow::copyTaskThreadsDetail()
{
	QList<QTableWidgetItem *> selectedItems;
	selectedItems = threadsDetailTableWidget->selectedItems();
	if (selectedItems.isEmpty()) return;
	QString clipboardString;
	for (int selectedItemsId=0;selectedItemsId<selectedItems.size()/2;selectedItemsId++)
	{
		QTableWidgetItem *item0 = selectedItems.at(selectedItemsId);
		QTableWidgetItem *item1 = selectedItems.at(selectedItemsId+selectedItems.size()/2);
		clipboardString += QString("%1 %2\r\n").arg(item0->text()).arg(item1->text());
	}
	clipboard->setText(clipboardString);
}
void MainWindow::createTaskThreadsDetailTableWidgetContextMenu()
{
	threadsDetailTableWidget->addAction(copyTaskThreadsDetailAct);
	threadsDetailTableWidget->setContextMenuPolicy(Qt::ActionsContextMenu);
}
void MainWindow::switchLanguage(QAction *action)
{
	QString locale = action->data().toString();

	QSettings settings;
	settings.beginGroup("MainWindow");
	settings.setValue("language", locale);
	settings.endGroup();

	QMessageBox::information(this, tr("Infomation"), tr("Take effect after restart software."));
}
void MainWindow::showApplicationNormal()
{
	showNormal();
	showGraphWidget(true);
}
void MainWindow::acceptClipboard()
{
	if (!taskPreferences.monitorClipboardBool) return;
	const QMimeData *mimeData = clipboard->mimeData();
	if (!mimeData->hasText()) return;

	bool allowedUrlBool = false;
	bool allowedTypesBool = false;
	QString string = mimeData->text();
	int n = string.indexOf(" ");
	if (n!=-1 && n<2048)
	{
		string = string.left(n);
	}
	n = string.indexOf("\'");
	if (n!=-1 && n<2048)
	{
		string = string.left(n);
	}
	n = string.indexOf("\"");
	if (n!=-1 && n<2048)
	{
		string = string.left(n);
	}
	n = string.indexOf(">");
	if (n!=-1 && n<2048)
	{
		string = string.left(n);
	}
	QStringList allowedUrlList;
	allowedUrlList<<"http://"<<"https://"<<"ftp://";
	for (int allowedUrlListId=0;allowedUrlListId<allowedUrlList.size();allowedUrlListId++)
	{
		if (string.left(allowedUrlList.at(allowedUrlListId).size()) == allowedUrlList.at(allowedUrlListId) )
		{
			allowedUrlBool = true;
			break;
		}
	}
	if (!allowedUrlBool) return;

	QStringList allowedTypesList = taskPreferences.monitorTypesString.split(";");
	for (int allowedTypesListId=0;allowedTypesListId<allowedTypesList.size();allowedTypesListId++)
	{
		n = QString::compare(string.right(allowedTypesList.at(allowedTypesListId).size()), allowedTypesList.at(allowedTypesListId), Qt::CaseInsensitive);
		if (n == 0)
		{
			allowedTypesBool = true;
			break;
		}
	}
	if (allowedUrlBool==true && allowedTypesBool==true)
	{
#if defined(DEBUG)
		qDebug()<<"clipboard add task:"<<string;
#endif
		newTask(string);
	}
}
