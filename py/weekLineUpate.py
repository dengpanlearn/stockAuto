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
        
        
    def prepareUpdate(self, saveDir):
        requests.packages.urllib3.disable_warnings(InsecureRequestWarning)
        self.session = requests.session()
        self.headers["Host"] = "xueqiu.com"
        self.saveDir = saveDir
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
            except (ReadTimeout, ConnectTimeout, ConnectionError):
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
        con=sqlite3.connect(self.saveDir+'\\stockList.db')
        pandas.io.sql.to_sql(df, 'code', con = con, if_exists='replace')


    def getAndUpdateWeekKLine(self, stockNo, beginTime, counts):
        stockKLine = self.getWeekKLine(stockNo, beginTime*1000, counts)
        
        if (stockKLine.get('error_code', 1)):
            return -1
        else :
            self.updateWeekKLine(stockKLine.get('data'))
            return 0
            
    def getWeekKLine(self, stockNo, beginTime, counts):
        orgUrl = 'https://stock.xueqiu.com/v5/stock/chart/kline.json?symbol={stockNo}&begin={beginTime}&period=week&type=before&count={counts}&indicator=kline,ma'
        url = orgUrl.format(stockNo=stockNo, beginTime = beginTime, counts=counts)
        self.headers["Host"] = "stock.xueqiu.com"
  
        try:
            response = self.session.get(url, verify=False, headers = self.headers)
        except (ReadTimeout, ConnectTimeout, ConnectionError):
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
        lastClose = 0
        itemList =  data.get('item')
        columns = data.get('column')

        closeIndex = columns.index('close')
        timestampIndex = columns.index('timestamp')
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
            timeVal = time.asctime(time.localtime(kline[timestampIndex]//1000))
            kline.append(timeVal)
            lastClose = kline[closeIndex]
            
        df = DataFrame(itemList, columns=columns)
        stockCode = data.get('symbol')
        con=sqlite3.connect(self.saveDir+'\\kline.db')
        pandas.io.sql.to_sql(df[['open','high', 'close', 'low', 'volume', 'rsi', 'ma10', 'chg', 'percent', 'timestamp', 'time']], stockCode, con = con, if_exists='replace')


if __name__ == '__main__':
    weekKLine =   WeekKLineUpdate()
    if (weekKLine.prepareUpdate('E:\\self\\stock\\data')):
        '''
        ret = weekKLine.getAndUpdateStockList(int(time.time()))
        '''
        ret = weekKLine.getAndUpdateWeekKLine('SZ000004', int(time.time()), -82)

