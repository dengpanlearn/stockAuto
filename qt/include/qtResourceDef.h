/*
*qtResourceDef.h
*/

#ifndef __QT_RESOURCE_DEF_H__
#define	__QT_RESOURCE_DEF_H__

#define	STOCK_AUTO_WINDOW_MEUNU_FUNC			"功能"
#define	STOCK_AUTO_WINDOW_ACTION_SETTING		"设置"
#define	STOCK_AUTO_WINDOW_ACTION_RESET_TRACE	"复位交易"
enum
{
	STOCK_REAL_WIDGET_INDEX_CODE = 0,
	STOCK_REAL_WIDGET_INDEX_NAME,
	STOCK_REAL_WIDGET_INDEX_CLOSE,
	STOCK_REAL_WIDGET_INDEX_OPEN,
	STOCK_REAL_WIDGET_INDEX_HIGH,
	STOCK_REAL_WIDGET_INDEX_LOW,
	STOCK_REAL_WIDGET_INDEX_MA10,
	STOCK_REAL_WIDGET_INDEX_RSI7,
	STOCK_REAL_WIDGET_INDEX_VOLUME,
	STOCK_REAL_WIDGET_INDEX_DATE,
	STOCK_REAL_WIDGET_INDEX_NUM,

	STOCK_REAL_WIDGET_TRACE_INDEX_CODE = 0,
	STOCK_REAL_WIDGET_TRACE_INDEX_NAME,
	STOCK_REAL_WIDGET_TRACE_INDEX_HIGHTIME,
	STOCK_REAL_WIDGET_TRACE_INDEX_HIGHVAL,
	STOCK_REAL_WIDGET_TRACE_INDEX_BUYTIME,
	STOCK_REAL_WIDGET_TRACE_INDEX_BUYVAL,
	STOCK_REAL_WIDGET_TRACE_INDEX_TOPTIME,
	STOCK_REAL_WIDGET_TRACE_INDEX_TOPVAL,
	STOCK_REAL_WIDGET_TRACE_INDEX_SELLTIME,
	STOCK_REAL_WIDGET_TRACE_INDEX_SELLVAL,
	STOCK_REAL_WIDGET_TRACE_INDEX_NUM
};


#define STOCK_REAL_WIDGET_TEXT_DFT		"--"
#define STOCK_REAL_WIDGET_LAB_CLOSE		"收盘价"
#define STOCK_REAL_WIDGET_LAB_OPEN		"开盘价"
#define STOCK_REAL_WIDGET_LAB_HIGH		"最高价"
#define STOCK_REAL_WIDGET_LAB_LOW		"最低价"
#define STOCK_REAL_WIDGET_LAB_MA10		"MA10"
#define STOCK_REAL_WIDGET_LAB_RSI7		"RSI7"
#define STOCK_REAL_WIDGET_LAB_VOLUME	"成交量"
#define STOCK_REAL_WIDGET_LAB_DATE		"日期"

#define STOCK_REAL_WIDGET_TRACE_TEXT_DFT STOCK_REAL_WIDGET_TEXT_DFT
#define  STOCK_REAL_WIDGET_TRACE_NAME_HIGHTIME "创新高时间"
#define  STOCK_REAL_WIDGET_TRACE_NAME_HIGHVAL "创新高价格"
#define  STOCK_REAL_WIDGET_TRACE_NAME_BUYTIME "买入时间"
#define  STOCK_REAL_WIDGET_TRACE_NAME_BUYVAL "买入价格"
#define  STOCK_REAL_WIDGET_TRACE_NAME_TOPTIME "顶点时间"
#define  STOCK_REAL_WIDGET_TRACE_NAME_TOPVAL "顶点价格"
#define  STOCK_REAL_WIDGET_TRACE_NAME_SELLTIME "卖出时间"
#define  STOCK_REAL_WIDGET_TRACE_NAME_SELLVAL "卖出价格"



#define STOCK_TRACE_WIDGET_LAB_WAIT_BUY		"等待买入股票"
#define STOCK_TRACE_WIDGET_LAB_WAIT_SELL	"等待卖出股票"

enum
{
	STOCK_TRACE_WIDGET_TREE_WAITBUY_INDEX_CODE_NAME = 0,
	STOCK_TRACE_WIDGET_TREE_WAITBUY_INDEX_RAISE_BALANCE,
	STOCK_TRACE_WIDGET_TREE_WAITBUY_INDEX_REACH_HIGH,
	STOCK_TRACE_WIDGET_TREE_WAITBUY_INDEX_WAIT_RSI,
	STOCK_TRACE_WIDGET_TREE_WAITBUY_INDEX_WAIT_BUY,
	STOCK_TRACE_WIDGET_TREE_WAITBUY_INDEX_NUM,
};

#define STOCK_TRACE_WIDGET_TREE_WAITBUY_NAME_CODE_NAME			"股票名称"
#define STOCK_TRACE_WIDGET_TREE_WAITBUY_NAME_RAISE_BALANCE		"横盘3周期"
#define STOCK_TRACE_WIDGET_TREE_WAITBUY_NAME_REACH_HIGH			"创新高"
#define STOCK_TRACE_WIDGET_TREE_WAITBUY_NAME_WAIT_RSI			"RSI破线"
#define STOCK_TRACE_WIDGET_TREE_WAITBUY_NAME_WAIT_BUY			"自动买入"


enum
{
	STOCK_TRACE_WIDGET_TREE_WAITSELL_INDEX_CODE_NAME = 0,
	STOCK_TRACE_WIDGET_TREE_WAITSELL_INDEX_MA10,
	STOCK_TRACE_WIDGET_TREE_WAITSELL_INDEX_EARNS,
	STOCK_TRACE_WIDGET_TREE_WAITSELL_INDEX_RSI,
	STOCK_TRACE_WIDGET_TREE_WAITSELL_INDEX_CUT_BACK,
	STOCK_TRACE_WIDGET_TREE_WAITSELL_INDEX_WAIT_SELL,
	STOCK_TRACE_WIDGET_TREE_WAITSELL_INDEX_NUM,
};

#define STOCK_TRACE_WIDGET_TREE_WAITSELL_NAME_CODE_NAME		"股票名称"
#define STOCK_TRACE_WIDGET_TREE_WAITSELL_NAME_MA10			"MA10/CLOSE"
#define STOCK_TRACE_WIDGET_TREE_WAITSELL_NAME_EARNS			"BUY/CLOSE/收益(%)"
#define STOCK_TRACE_WIDGET_TREE_WAITSELL_NAME_RSI			"RSI"
#define STOCK_TRACE_WIDGET_TREE_WAITSELL_NAME_CUT_BACK		"买入后HIGH/CLOSE/回落比(%)"
#define STOCK_TRACE_WIDGET_TREE_WAITSELL_NAME_WAIT_SELL		"自动卖出"


#define	STOCK_HIS_WIDGET_BTN_SEARCH							"周线查询"
enum
{
	STOCK_HIS_WIDGET_TREE_HISKLINE_INDEX_DATE = 0,
	STOCK_HIS_WIDGET_TREE_HISKLINE_INDEX_HIGH,
	STOCK_HIS_WIDGET_TREE_HISKLINE_INDEX_LOW,
	STOCK_HIS_WIDGET_TREE_HISKLINE_INDEX_CLOSE,
	STOCK_HIS_WIDGET_TREE_HISKLINE_INDEX_OPEN,
	STOCK_HIS_WIDGET_TREE_HISKLINE_INDEX_MA10,
	STOCK_HIS_WIDGET_TREE_HISKLINE_INDEX_RSI,
	STOCK_HIS_WIDGET_TREE_HISKLINE_INDEX_PERCENT,
	STOCK_HIS_WIDGET_TREE_HISKLINE_INDEX_VOL,
	STOCK_HIS_WIDGET_TREE_HISKLINE_INDEX_NUM,
};

#define	STOCK_HIS_WIDGET_TREE_HISKLINE_NAME_DATE		"日期"
#define	STOCK_HIS_WIDGET_TREE_HISKLINE_NAME_HIGH		"HIGH"
#define	STOCK_HIS_WIDGET_TREE_HISKLINE_NAME_LOW			"LOW"
#define	STOCK_HIS_WIDGET_TREE_HISKLINE_NAME_CLOSE		"CLOSE"
#define	STOCK_HIS_WIDGET_TREE_HISKLINE_NAME_OPEN		"OPEN"
#define	STOCK_HIS_WIDGET_TREE_HISKLINE_NAME_MA10		"MA10"
#define	STOCK_HIS_WIDGET_TREE_HISKLINE_NAME_RSI			"RSI"
#define	STOCK_HIS_WIDGET_TREE_HISKLINE_NAME_PERCENT		"PERCENT"
#define	STOCK_HIS_WIDGET_TREE_HISKLINE_NAME_VOL			"VOLUME"

#define	STOCK_LOADING_DIALOG_LAB_STAT_INIT				"初始化中..."
#define	STOCK_LOADING_DIALOG_LAB_STAT_LIST_LOADING		"获取股票列表"
#define	STOCK_LOADING_DIALOG_LAB_STAT_LIST_UPDATING		"更新股票列表"
#define	STOCK_LOADING_DIALOG_LAB_STAT_TRACELOG_LOADING	"获取股票交易状态"
#define	STOCK_LOADING_DIALOG_LAB_STAT_HISKLINE_UPDATING	"获取股票KLINE"
#define	STOCK_LOADING_DIALOG_LAB_STAT_STOCK_TRACING		"股票自动交易开始"
#define	STOCK_LOADING_DIALOG_LAB_STAT_ERROR				"登录失败"

#define	STOCK_SETTING_DIALOG_EDIT_PYTHON_NAME				"PYTHON配置"
#define	STOCK_SETTING_DIALOG_EDIT_PYTHON_MODULE_DIR_NAME	"模块路径"
#define	STOCK_SETTING_DIALOG_EDIT_PYTHON_KLINE_MODULE_NAME	"模块名称"
#define	STOCK_SETTING_DIALOG_EDIT_PYTHON_UPDATE_DIR_NAME	"数据更新路径"
#define STOCK_SETTING_DIALOG_EDIT_PYTHON_DB_STOCKLIST_NAME	"股票列表数据库名"
#define STOCK_SETTING_DIALOG_EDIT_PYTHON_DB_STOCKKLINE_NAME	"股票KLINE数据库名"

#define	STOCK_SETTING_DIALOG_EDIT_DATA_NAME						"数据库配置"
#define	STOCK_SETTING_DIALOG_EDIT_DATA_DATA_DIR_NAME			"数据库路径"
#define	STOCK_SETTING_DIALOG_EDIT_DATA_LIST_FILENAMEF_NAME		"股票列表数据库名"
#define	STOCK_SETTING_DIALOG_EDIT_DATA_KLINE_FILENAME_NAME		"股票KLINE数据库名"
#define	STOCK_SETTING_DIALOG_EDIT_DATA_TRACELOG_FILENAME_NAME	"股票交易数据库名"

#define	STOCK_SETTING_DIALOG_EDIT_TRACE_NAME					"交易配置"
#define	STOCK_SETTING_DIALOG_EDIT_TRACE_RAISE_PERCENT_NAME		"横盘涨幅％"
#define	STOCK_SETTING_DIALOG_EDIT_TRACE_RAISE_BALANCES_NAME		"横盘周期数"
#define	STOCK_SETTING_DIALOG_EDIT_TRACEREACHHIGH_RANGES_NAME	"创新高周期数"
#define	STOCK_SETTING_DIALOG_EDIT_TRACE_RSIBUY_WAITS_NAME		"等待买入RSI周期数"
#define	STOCK_SETTING_DIALOG_EDIT_TRACE_RSIBUY_NAME				"买入RSI值"
#define	STOCK_SETTING_DIALOG_EDIT_TRACE_RSISELL_NAME			"卖出RSI值"
#define	STOCK_SETTING_DIALOG_EDIT_TRACE_RSISELL_WAITS_NAME		"等待卖出RSI周期数"
#define	STOCK_SETTING_DIALOG_EDIT_TRACE_CUTLOSS_PERCENT_NAME	"止损％"
#define	STOCK_SETTING_DIALOG_EDIT_TRACE_CUTLOSS_AFTERTOP_NAME	"顶点止损％"
enum
{
	STOCK_SETTING_DIALOG_EDIT_PYTHON_IDX_MODULE_DIR = 0x00,
	STOCK_SETTING_DIALOG_EDIT_PYTHON_IDX_KLINE_MODULE,
	STOCK_SETTING_DIALOG_EDIT_PYTHON_IDX_UPDATE_DIR,
	STOCK_SETTING_DIALOG_EDIT_PYTHON_IDX_DB_STOCKLIST,
	STOCK_SETTING_DIALOG_EDIT_PYTHON_IDX_DB_STOCKKLINE,
	STOCK_SETTING_DIALOG_EDIT_PYTHON_IDX_NUM,

	STOCK_SETTING_DIALOG_EDIT_DATA_IDX_DATA_DIR = 0x00,
	STOCK_SETTING_DIALOG_EDIT_DATA_IDX_LIST_FILENAME,
	STOCK_SETTING_DIALOG_EDIT_DATA_IDX_KLINE_FILENAME,
	STOCK_SETTING_DIALOG_EDIT_DATA_IDX_TRACELOG_FILENAME,
	STOCK_SETTING_DIALOG_EDIT_DATA_IDX_NUM,

	STOCK_SETTING_DIALOG_EDIT_TRACE_IDX_RAISE_PERCENT = 0x00,
	STOCK_SETTING_DIALOG_EDIT_TRACE_IDX_RAISE_BALANCES,
	STOCK_SETTING_DIALOG_EDIT_TRACE_IDX_REACHHIGH_RANGES,
	STOCK_SETTING_DIALOG_EDIT_TRACE_IDX_RSIBUY_WAITS,
	STOCK_SETTING_DIALOG_EDIT_TRACE_IDX_RSIBUY,
	STOCK_SETTING_DIALOG_EDIT_TRACE_IDX_RSISELL,
	STOCK_SETTING_DIALOG_EDIT_TRACE_IDX_RSISELL_WAITS,
	STOCK_SETTING_DIALOG_EDIT_TRACE_IDX_CUTLOSS_PERCENT,
	STOCK_SETTING_DIALOG_EDIT_TRACE_IDX_CUTLOSS_AFTERTOP,
	STOCK_SETTING_DIALOG_EDIT_DTRACE_IDX_NUM
};

#define	STOCK_SETTING_DIALOG_SELECT_PYTHON_DIR		"选择python模块路径"
#define	STOCK_SETTING_DIALOG_SELECT_DATA_DIR		"选择数据库路径"
#define STOCK_SETTING_DIALOG_BTN_SAVE				"保存"
#define STOCK_SETTING_DIALOG_BTN_CANCLE				"取消"


enum
{
	STOCK_TRACE_INFO_WIDGET_INDEX_CODE = 0x00,
	STOCK_TRACE_INFO_WIDGET_INDEX_HIGHTIME,
	STOCK_TRACE_INFO_WIDGET_INDEX_HIGHVAL,
	STOCK_TRACE_INFO_WIDGET_INDEX_BUYTIME,
	STOCK_TRACE_INFO_WIDGET_INDEX_BUYVAL,
	STOCK_TRACE_INFO_WIDGET_INDEX_SELLTIME,
	STOCK_TRACE_INFO_WIDGET_INDEX_SELLVAL,
	STOCK_TRACE_INFO_WIDGET_INDEX_NUM
};

#endif // !__QT_RESOURCE_DEF_H__
