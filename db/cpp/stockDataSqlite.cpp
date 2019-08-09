/*
*stockDataSqlite.cpp*/

#include <qdir.h>
#include <qtGlobal.h>
#include <qtextcodec.h>
#include <qsqlquery.h>
#include "../include/stockDataSqlite.h"


BOOL CStockDataSqlite::Init(const char* dbDir, const char* pListName, const char* pKLineName)
{
	QTextCodec* pTextCode = QTextCodec::codecForLocale();
	QString dirPath = pTextCode->toUnicode(dbDir);
	QDir dir(dirPath);

	QString listFile = dir.absoluteFilePath(pTextCode->toUnicode(pListName));
	QString klineFile = dir.absoluteFilePath(pTextCode->toUnicode(pKLineName));

	m_listDb = QSqlDatabase::addDatabase("QSQLITE", "stocklist");
	m_listDb.setDatabaseName(listFile);


	m_klineDb = QSqlDatabase::addDatabase("QSQLITE", "kline");
	m_klineDb.setDatabaseName(klineFile);

	m_listDb.open();
	m_klineDb.open();

	return TRUE;
}

void CStockDataSqlite::Close()
{
	m_listDb.close();
	m_klineDb.close();
}

BOOL CStockDataSqlite::StockListIsOn()
{
	return m_listDb.isOpen();
}

int CStockDataSqlite::GetStockList(STOCK_CODE_NAME* pListBuf, int bufCounts)
{
	if (!m_listDb.isOpen())
		return -1;

	QSqlQuery sqlQuery;
	QString selSql("select symbol, name from code");
	if (!sqlQuery.exec(selSql))
	{
		return -1;
	}

	int stockCounts = 0;
	while (sqlQuery.next() && bufCounts--)
	{
		QString code= sqlQuery.value(0).toString();
		QString name = sqlQuery.value(1).toString();

		QString2Char(code, pListBuf->code);
		QString2Char(name, pListBuf->name);
		stockCounts++;
	}

	return stockCounts;
}