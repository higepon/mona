package java.util;

/**
 日付と時間のクラス
*/
public class Date {
	public int year, month, date, hour, minutes, second;
	
	private native void create(Date date);
	
	/** デフォルトコンストラクタ */
	public Date() {
		create(this);
	}
	
	/** 年を得る */
	public int getYear() {
		return this.year;
	}
	
	/** 月を得る(1-12) */
	public int getMonth() {
		return this.month;
	}
	
	/** 日を得る(1-31) */
	public int getDate() {
		return this.date;
	}
	
	/** 時を得る(1-24) */
	public int getHours() {
		return this.hour;
	}
	
	/** 分を得る(1-60) */
	public int getMinutes() {
		return this.minutes;
	}
	
	/** 秒を得る(1-60) */
	public int getSeconds() {
		return this.second;
	}
	
	/** 週の曜日を得る(0-6) */
	public int getDay() {
		int y = this.year;
		int m = this.month;
		int d = this.date;
		
		// ツェラーの公式
		
		if ((m == 1) || (m == 2)) {
			y--;
			m += 12;
		}
		
		return ((y + (int)(y/4) - (int)(y/100) + (int)(y/400) + (int)((13 * m + 8)/5) + d) % 7);
	}
}
