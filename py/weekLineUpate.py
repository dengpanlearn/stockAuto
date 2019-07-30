import requests
import pandas
import json
import sqlite3
import math

from pandas import DataFrame
from requests.exceptions import ReadTimeout
from requests.exceptions import ConnectTimeout
from requests.exceptions import ConnectionError
from requests.packages.urllib3.exceptions import InsecureRequestWarning

class WeekKLineUpdate:
    def __init__(self, saveDir):
        self.headers = {"Accept":"text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3",
                        "Accept-Encoding":"gzip, deflate, br",
                        "Host": "xueqiu.com",
                        "User-Agent":"Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/75.0.3770.142 Safari/537.36"
                    }
        self.hostUrl = 'https://xueqiu.com'
        self.saveDir = saveDir
        
    def prepareUpdate(self):
        requests.packages.urllib3.disable_warnings(InsecureRequestWarning)
        self.session = requests.session()
        try:
            response = self.session.get(self.hostUrl,verify=False, headers= self.headers)
        except (ReadTimeout, ConnectTimeout, ConnectionError):
            return False
        else:
            if (response.status_code == 200):
                return True
            else:
                return False

    def getStockList(self, curTime):
        outDf = DataFrame(columns=['symbol','name'])
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
            
    def getWeekKLine(self, stockNo, ticksStartMs, ticksEndMs):
        orgUrl = 'https://xueqiu.com/stock/forchartk/stocklist.json?symbol={stockNo}&period=1week&type=before&begin={beginMs}&end={endMs}'
        url = orgUrl.format(stockNo=stockNo, beginMs=ticksStartMs, endMs=ticksEndMs)
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
        charList =  data.get('chartlist')
        for kline in charList :
            difClose = kline['close']-lastClose
            curSmaAbs += (math.fabs(difClose)-curSmaAbs)/rsiCycle
            if (difClose > 0) :
                curSmaGain += (difClose-curSmaGain)/rsiCycle
            else :
                curSmaGain += (0-curSmaGain)/rsiCycle
            kline['rsi'] = curSmaGain*100/curSmaAbs
            lastClose = kline['close']
            
        df = DataFrame(charList)
        stockCode = data.get('stock').get('symbol')
        con=sqlite3.connect(self.saveDir+'\\kline.db')
        pandas.io.sql.to_sql(df[['open','high', 'close', 'low', 'volume', 'rsi', 'ma10', 'chg', 'percent', 'timestamp']], stockCode, con = con, if_exists='replace')


if __name__ == '__main__':
    weekKLine =   WeekKLineUpdate('E:\\self\\stock\\data')
    if (weekKLine.prepareUpdate()):
        dfList = weekKLine.getStockList(1564560000000)
        if (not dfList.empty):
            weekKLine.updateStockList(dfList)
        stockKLine = weekKLine.getWeekKLine('sz000005', 1514102400000,1564560000000)
        if (stockKLine):
            weekKLine.updateWeekKLine(stockKLine )
