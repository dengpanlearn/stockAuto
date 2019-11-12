import requests
import numpy
import pandas
import json
import sqlite3
import math
import time

from pandas import DataFrame
from requests.exceptions import ReadTimeout
from requests.exceptions import ConnectTimeout
from requests.exceptions import TooManyRedirects
from requests.exceptions import ConnectionError
from requests.packages.urllib3.exceptions import InsecureRequestWarning

class WeekKLineUpdate:
    def __init__(self):
        self.headers = {"Accept":"text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3",
                        "Accept-Encoding":"gzip, deflate, br",
                        "Host": "xueqiu.com",
                        "User-Agent":"Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/75.0.3770.142 Safari/537.36"
                    }
        self.hostUrl = 'https://xueqiu.com'
        requests.packages.urllib3.disable_warnings(InsecureRequestWarning)
        self.session = requests.session()
        
    def prepareUpdate(self, saveDir, dbStockList, dbStockKLine):
        self.headers["Host"] = "xueqiu.com"
        self.saveDir = saveDir
        self.dbStockList = dbStockList
        self.dbStockKLine = dbStockKLine
        try:
            response = self.session.get(self.hostUrl,verify=False, headers= self.headers)
        except (ReadTimeout, ConnectTimeout, ConnectionError):
            return -1
        else:
            if (response.status_code == 200):
                return 0
            else:
                return -1

    def getAndUpdateStockList(self, curTime):
        dfList = self.getStockList(curTime*100)
        if (dfList.empty):
            return -1
        else :
            self.updateStockList(dfList)
            return 0
            
    def getStockList(self, curTime):
        outDf = DataFrame(columns=['symbol','name'])
        self.headers["Host"] = "xueqiu.com"
        orgUrl = 'https://xueqiu.com/service/v5/stock/screener/quote/list?page={pageNo}&size=90&order=asc&orderby=code&order_by=symbol&market=CN&type=sh_sz&_={curTime}'
        
        for k in range(1, 50):
            url = orgUrl.format(pageNo=k, curTime = curTime)
            try:
                response = self.session.get(url, verify=False, headers = self.headers)
            except (ReadTimeout, ConnectTimeout, ConnectionError, TooManyRedirects):
                break
            else:
                if (response.status_code == 200):
                    orgList = response.json().get('data').get('list')
                    if (orgList):
                        dfPage = DataFrame(orgList)
                        dfPageExract = dfPage[['symbol','name']]
                        outDf = outDf.append(dfPageExract, ignore_index = True)
                    else:
                        break
                else:
                    break
                
        return outDf

    def updateStockList(self, df):
        con=sqlite3.connect(self.saveDir+'\\'+self.dbStockList)
        pandas.io.sql.to_sql(df, 'code', con = con, if_exists='replace')


    def getCurWeekKLine(self, stockNo):
        stockKLine = self.getWeekKLine(stockNo, int(time.time())*1000, -80)
        if (not stockKLine):
            return {'code':-1,'item':None}
        if (stockKLine.get('error_code', 1)):
            return {'code':-1,'item':None}
        else :
            if (not stockKLine.get("data")):
                return {'code':-1,'item':None}
            curKLineVal = self.calcCurWeekKLine(stockKLine.get('data'))
            if curKLineVal:
                return {'code':0, 'item':curKLineVal}
            else:
                return {'code':-1,'item':None}

    def calcCurWeekKLine(self, data):
        curSmaAbs = 0
        curSmaGain = 0
        rsiCycle = 7
        lastClose = 0.1
        itemList =  data.get('item')
        columns = data.get('column')

        try:
            openIndex = columns.index('open')
            closeIndex = columns.index('close')
            highIndex = columns.index('high')
            lowIndex = columns.index('low')
            percentIndex = columns.index('percent')
            ma10Index = columns.index('ma10')
            timestampIndex = columns.index('timestamp')
            
            for kline in itemList :
                difClose = kline[closeIndex]-lastClose
                kline[percentIndex]=(difClose*100)/lastClose
                curSmaAbs += (math.fabs(difClose)-curSmaAbs)/rsiCycle
                if (difClose > 0) :
                    curSmaGain += (difClose-curSmaGain)/rsiCycle
                else :
                    curSmaGain += (0-curSmaGain)/rsiCycle
                kline.append(curSmaGain*100/curSmaAbs)
                lastClose = kline[closeIndex]
                
           
            return [itemList[-1][openIndex], itemList[-1][closeIndex], itemList[-1][highIndex], itemList[-1][lowIndex],itemList[-1][percentIndex],itemList[-1][ma10Index], itemList[-1][timestampIndex]//1000,
                    itemList[-1][-1]]
        except ValueError:
            return None

            
    def getAndUpdateWeekKLine(self, stockNo, beginTime, counts):
        stockKLine = self.getWeekKLine(stockNo, beginTime*1000, counts)
        if (not stockKLine):
            return -1
        if (stockKLine.get('error_code', 1)):
            return -1
        else :
            if (not stockKLine.get("data")):
                return -1
            if self.updateWeekKLine(stockKLine.get('data')):
                return 0
            else :
                return -1
            
    def getWeekKLine(self, stockNo, beginTime, counts):
        orgUrl = 'https://stock.xueqiu.com/v5/stock/chart/kline.json?symbol={stockNo}&begin={beginTime}&period=week&type=before&count={counts}&indicator=kline,ma'
        url = orgUrl.format(stockNo=stockNo, beginTime = beginTime, counts=counts)
        self.headers["Host"] = "stock.xueqiu.com"
  
        try:
            response = self.session.get(url, verify=False, headers = self.headers)
        except (ReadTimeout, ConnectTimeout, ConnectionError, TooManyRedirects):
            return None;
        else:
            if (response.status_code == 200):
                return response.json()
            else:
                return None

    def updateWeekKLine(self, data):
        curSmaAbs = 0
        curSmaGain = 0
        rsiCycle = 7
        lastClose = 0.1
        itemList =  data.get('item')
        columns = data.get('column')

        try:
            closeIndex = columns.index('close')
            timestampIndex = columns.index('timestamp')
            percentIndex = columns.index('percent')
            columns.append('rsi')
            columns.append('time')
            for kline in itemList :
                difClose = kline[closeIndex]-lastClose
                curSmaAbs += (math.fabs(difClose)-curSmaAbs)/rsiCycle
                if (difClose > 0) :
                    curSmaGain += (difClose-curSmaGain)/rsiCycle
                else :
                    curSmaGain += (0-curSmaGain)/rsiCycle
                kline.append(curSmaGain*100/curSmaAbs)
                kline[timestampIndex] =  kline[timestampIndex]//1000
                timeVal = time.asctime(time.localtime(kline[timestampIndex]))
                kline[percentIndex] = difClose*100/lastClose
                kline.append(timeVal)
                lastClose = kline[closeIndex]
                
            df = DataFrame(itemList, columns=columns)
            stockCode = data.get('symbol')
            con=sqlite3.connect(self.saveDir+'\\'+self.dbStockKLine)
            pandas.io.sql.to_sql(df[['open','high', 'close', 'low', 'volume', 'rsi', 'ma10', 'chg', 'percent', 'timestamp', 'time']], stockCode, con = con, if_exists='replace')
            return True
        except ValueError:
            return False

if __name__ == '__main__':
    weekKLine =   WeekKLineUpdate()
    if (not weekKLine.prepareUpdate('E:\\self\\stock\\data', 'stockList.db', 'kline.db')):
        """
        ret = weekKLine.getAndUpdateStockList(int(time.time()))
      
        ret = weekKLine.getAndUpdateWeekKLine('SZ300599', int(time.time()), -1)
       
        ret = weekKLine.getCurWeekKLine('SZ300001')
        """
        ret = weekKLine.getCurWeekKLine('SZ200488')
