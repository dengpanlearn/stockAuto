import requests
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
    def __init__(self, saveDir):
        self.headers = {"Accept":"text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3",
                        "Accept-Encoding":"gzip, deflate, br",
                        "Host": "xueqiu.com",
                        "User-Agent":"Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/75.0.3770.142 Safari/537.36",
                        "Cookie": "_ga=GA1.2.1914090483.1551762674; device_id=a9e2d76ee0e6160e8ba604a7e2cdc7bc; s=cp12g5xoxo; __utmz=1.1560230558.1.1.utmcsr=(direct)|utmccn=(direct)|utmcmd=(none);\
                        Hm_lvt_1db88642e346389874251b5a1eded6e3=1564039732,1564384833; _gid=GA1.2.1561277100.1564389468; __utma=1.1914090483.1551762674.1564390710.1564466946.6;\
                        aliyungf_tc=AQAAACz0rmxBtAUAgn1Tq2V9/bh64GgV;xq_a_token=d795cb8533055e91fb68a4a16905a27ccab77258; xqat=d795cb8533055e91fb68a4a16905a27ccab77258;\
                        xq_r_token=8edd6c2dc7ff85ed2113c6a92f853cd7641c9eb5; u=211564472697042"
                    }
        self.hostUrl = 'https://xueqiu.com'
        self.saveDir = saveDir
        
    def prepareUpdate(self):
        requests.packages.urllib3.disable_warnings(InsecureRequestWarning)
        self.session = requests.session()
        self.headers["Host"] = "xueqiu.com"
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
    weekKLine =   WeekKLineUpdate('E:\\self\\stock\\data')
    if (weekKLine.prepareUpdate()):
        '''
        dfList = weekKLine.getStockList(1564560000000)
        if (not dfList.empty):
            weekKLine.updateStockList(dfList)
        '''
        stockKLine = weekKLine.getWeekKLine('SZ000004', int(time.time()*1000), -82)
        if (stockKLine):
            weekKLine.updateWeekKLine(stockKLine.get('data'))
