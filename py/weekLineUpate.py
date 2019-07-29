import requests
import pandas
import json
import sqlite3

from pandas import DataFrame
from requests.exceptions import ReadTimeout
from requests.exceptions import ConnectTimeout
from requests.exceptions import ConnectionError
from requests.packages.urllib3.exceptions import InsecureRequestWarning

class WeekKLineUpdate:
    def __init__(self, dbFile):
        self.headers = {"Accept":"text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3",
                        "Accept-Encoding":"gzip, deflate, br",
                        "Host": "xueqiu.com",
                        "User-Agent":"Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/75.0.3770.142 Safari/537.36"
                    }
        self.hostUrl = 'https://xueqiu.com'
        self.dbFile = dbFile
        
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
        orgUrl = 'https://xueqiu.com/service/v5/stock/screener/quote/list?page={pageNo}&size=90&order=asc&orderby=code&order_by=symbol&market=CN&type=sh_sz&_={curTime}'
        for k in range(1, 50):
            url = orgUrl.format(pageNo=k, curTime = curTime)
            try:
                respone = self.session.get(url, verify=False, headers = self.headers)
            except (ReadTimeout, ConnectTimeout, ConnectionError):
                break
            else:
                if (response.status_code == 200):
                    return response.json()
                else:
                    break
            
    def getWeekKLine(self, stockNo, ticksStartMs, ticksEndMs):
        stockList = []
        orgUrl = 'https://xueqiu.com/stock/forchartk/stocklist.json?symbol={stockNo}&period=1week&type=before&begin={beginMs}&end={endMs}'
        url = orgUrl.format(stockNo=stockNo, beginMs=ticksStartMs, endMs=ticksEndMs)
        try:
            response = self.session.get(url, verify=False, headers = self.headers)
        except (ReadTimeout, ConnectTimeout, ConnectionError):
            break;
        else:
            if (response.status_code == 200):
                orgList = response.json().get('data').get('list')
                if (orgList):
                    dfPage = DataFrame(orgList)
                else:
                    break;
            else:
                break;

    def updateWeekKLine(self, data):
        df = DataFrame(data.get('chartlist'))
        stockCode = data.get('stock').get('symbol')
        con=sqlite3.connect(self.dbFile)
        pandas.io.sql.to_sql(df, stockCode, con = con, if_exists='replace')


if __name__ == '__main__':
    weekKLine =   WeekKLineUpdate('E:\\self\\stock\\data\\kline.db')
    if (weekKLine.prepareUpdate()):
        stockKLine = weekKLine.getWeekKLine('sz000004', 1514102400000,1564560000000)
        if (stockKLine):
            weekKLine.updateWeekKLine(stockKLine )
