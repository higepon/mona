//*****************************************************************************
// fat.cpp :
// 2004/02/02 by Gaku :
// 2006/03/25 by Yume : endian for MacOSX
// Licence : see http://gaku.s12.xrea.com/wiki/main.cgi?c=g&p=Gaku%2FLicence
//*****************************************************************************
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifndef MONA
#include <stdio.h>	
#include <stdlib.h>
#include <string.h>
#endif

#include <string.h>
#include "fat.h"
using namespace FatFS;
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// FAT
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

//-----------------------------------------------------------------------------
const uint8_t FAT::FileSystemID[] = "FAT12   ";

//=============================================================================
FAT::FAT ()
{
	fat = NULL;
}

//=============================================================================
FAT::~FAT ()
{
	delete[] fat;
}

//=============================================================================
//この関数は直したはず
bool FAT::initialize (IStorageDevice *p)
{
	uint8_t bf[SECTOR_SIZE];

	bytesPerSector = SECTOR_SIZE;
	floppy = p;
	if (false == read(0, bf))
		return false;

	// File System ID を確認する
	if (0 != memcmp(bf + FILE_SYSTEM_ID, FileSystemID, 8))
		return false;

	// BIOS Parameter Block の情報を読み込む
	uint32_t numberOfSectors;

	bytesPerSector    = little2host16(*((uint16_t*)( bf + BYTES_PER_SECTOR )));
	sectorsPerCluster = *((uint8_t*)( bf + SECTORS_PER_CLUSTER ));
	reservedSectors   = little2host16(*((uint16_t*)( bf + RESERVED_SECTORS )));
	numberOfFats      = *((uint8_t*)( bf + NUMBER_OF_FATS      ));
	numberOfDirEntry  = little2host16(*((uint16_t*)( bf + NUMBER_OF_DIRENTRY)));
	numberOfSectors   = little2host16(*((uint16_t*)( bf + NUMBER_OF_SECTORS )));
	sectorsPerFat     = little2host16(*((uint16_t*)( bf + SECTORS_PER_FAT   )));

	uint32_t bytesPerFat = bytesPerSector * sectorsPerFat;
	uint32_t sectorsPerDirEntry = numberOfDirEntry / ( bytesPerSector / 0x20 );

	rootDirectoryEntry = reservedSectors + sectorsPerFat * numberOfFats;
	dataArea = rootDirectoryEntry + sectorsPerDirEntry;
	numberOfClusters = numberOfSectors - dataArea;

	// メモリ確保
	uint8_t *ptr = new uint8_t [bytesPerFat + sectorsPerFat];
	if (NULL == ptr)
		return false;

	// File Allocation Table の読み込み
	uint8_t *tmp = ptr;
	for (uint32_t n = 0; n < sectorsPerFat; n++) {
		if (false == read(reservedSectors + n, tmp)) {
			delete[] ptr;
			return false;
		}
		tmp += bytesPerSector;
	}

	fat = ptr;
	flag = ptr + bytesPerFat;

	return true;
}

//=============================================================================
uint32_t FAT::getBytesPerSector ()
{
	return bytesPerSector;
}

//=============================================================================
uint32_t FAT::getSectorsPerCluster ()
{
	return sectorsPerCluster;
}

//=============================================================================
uint32_t FAT::getNumberOfDirEntry ()
{
	return numberOfDirEntry;
}

//=============================================================================
uint32_t FAT::getNumberOfClusters ()
{
	return numberOfClusters;
}

//=============================================================================
uint32_t FAT::getRootDirectoryEntry ()
{
	return rootDirectoryEntry;
}

//=============================================================================
uint8_t* FAT::readSectors (uint32_t c, uint32_t s, uint32_t d, uint32_t *sects, uint32_t *last)
{
	uint32_t num = getSectorsPerCluster();
	uint32_t bsize = getBytesPerSector();

	uint32_t *lba = new uint32_t [s];
	if (NULL == lba)
		return NULL;

	uint32_t i = 0;
	uint32_t l = 0;

	// FAT の読み込み
	while (getNumberOfClusters() > c) {
		uint32_t base = getLbaFromCluster(c);

		if (s < i + num) {
			// サイズが足りなくなったのでメモリの拡大
			uint32_t tmp_size = s + d;
			uint32_t *tmp = new uint32_t [tmp_size];
			if (NULL == tmp) {
				delete[] lba;
				return NULL;
			}

			memcpy(tmp, lba, s * sizeof(uint32_t));

			delete[] lba;

			lba = tmp;
			s = tmp_size;
		}

		for (uint32_t n = 0; n < num; n++) 
			lba[i++] = base+n;

		l = c;
		c = getNextCluster(c);
	}

	// セクタの読み込み
	// セクタ本体 + セクタインデックス + セクタ操作フラグ
	uint32_t size = bsize * i + sizeof(uint32_t) * i + i;

	uint8_t *ptr = new uint8_t [size];
	if (NULL == ptr) {
		delete[] lba;
		return NULL;
	}

	uint8_t *tmp = ptr;

	for (uint32_t n = 0; n < i; n++) {
		if (false == read(lba[n], tmp)) {
			delete[] lba;
			delete[] ptr;
			return NULL;
		}
		tmp += bsize;
	}

	memcpy(tmp, lba, i * sizeof(uint32_t));

	delete[] lba;

	*sects = i;
	*last = l;

	return ptr;
}

//=============================================================================
uint32_t FAT::allocateCluster (uint32_t cluster, uint32_t count)
{
	uint32_t next, temp;

	// クラスタの新規確保
	if (0 == cluster) {
		cluster = searchFreeCluster(START_OF_CLUSTER);
		if (0 == cluster)
			return 0;
		// 要求数がゼロでも新規作成の場合は最低一つは割り当てる
		if (0 < count)
			count--;
	}

	// クラスタを要求数だけ続けて確保
	next = cluster;
	while (0 < count--) {
		temp = searchFreeCluster(next+1);
		if (0 == temp) {
			freeCluster(cluster);
			return 0;
		}

		setNextCluster(next, temp);
		next = temp;
	}
	setNextCluster(next, END_OF_CLUSTER);

	// FAT の書き戻し
	flushFat();

	return cluster;
}

//=============================================================================
void FAT::freeCluster (uint32_t cluster)
{
	uint32_t next;

	do {
		next = getNextCluster(cluster);
		setNextCluster(cluster, 0);
		cluster = next;
	} while (numberOfClusters > cluster);

	// FAT の書き戻し
	flushFat();
}

//=============================================================================
void FAT::setEndOfCluster (uint32_t cluster)
{
	setNextCluster(cluster, END_OF_CLUSTER);
}

//=============================================================================
bool FAT::read (uint32_t lba, uint8_t *bf)
{
	floppy->read(lba, bf, bytesPerSector);

	return true;
}

//=============================================================================
bool FAT::write (uint32_t lba, uint8_t *bf)
{
	floppy->write(lba, bf, bytesPerSector);

	return true;
}

//=============================================================================
uint32_t FAT::getLbaFromCluster (uint32_t cluster)
{
	return dataArea + sectorsPerCluster * ( cluster - 2 );
}

//=============================================================================
uint32_t FAT::getClusterFromLba (uint32_t lba)
{
	return ( lba - dataArea ) / sectorsPerCluster;
}

//=============================================================================
uint32_t FAT::getNextCluster (uint32_t cluster)
{
	uint32_t index = cluster + cluster / 2;
	uint32_t next;

	if (cluster & 0x01) {
		next = ( fat[index+1] << 4 ) + ( fat[index] >> 4 );
	} else {
		next = ( (fat[index+1] & 0x0f) << 8 ) + fat[index];
	}

	if (next == cluster)
		return END_OF_CLUSTER;

	return next;
}

//-----------------------------------------------------------------------------
void FAT::setNextCluster (uint32_t cluster, uint32_t next)
{
	uint32_t index = cluster + cluster / 2;

	if (cluster & 0x01) {
		fat[index+1] = next >> 4;
		fat[index] = (fat[index] & 0x0f) | (next & 0x0f) << 4;
	} else {
		fat[index+1] = (fat[index+1] & 0xf0) | (next >> 8);
		fat[index] = next;
	}

	// 変更したセクタにはフラグを立てておく
	flag[cluster * 3 / ( 2 * bytesPerSector ) ] = 1;
	flag[ ( cluster * 3 + 2 ) / ( 2 * bytesPerSector ) ] = 1;
}

//-----------------------------------------------------------------------------
uint32_t FAT::searchFreeCluster (uint32_t cluster)
{
	// 大きすぎるクラスタ番号が来たら、検索開始位置を先頭へ戻す
	if (numberOfClusters <= cluster)
		cluster = START_OF_CLUSTER;
	uint32_t start = cluster;

	do {
		// クラスタが空いているかを調べていく
		uint32_t next = getNextCluster(cluster);
		if (0 == next)
			return cluster;
		cluster++;

		// 末尾まで来たら、先頭から調べなおす
		if (numberOfClusters <= cluster)
			cluster = START_OF_CLUSTER;
	} while (start != cluster);

	return 0;
}

//-----------------------------------------------------------------------------
void FAT::clearFlag ()
{
	for (uint32_t n = 0; n < sectorsPerFat; n++) {
		flag[n] = 0;
	}
}

//-----------------------------------------------------------------------------
void FAT::flushFat ()
{
	// フラグが立っているセクタをディスクに書き戻す
	for (uint32_t n = 0; n < sectorsPerFat; n++) {
		if (0 != flag[n]) {
			uint8_t *tmp = fat + n * bytesPerSector;
			write(reservedSectors + n, tmp);
			write(reservedSectors + sectorsPerFat + n, tmp);
			flag[n] = 0;
		}
	}
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// FatFile
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

//=============================================================================
FatFile::FatFile ()
{
	file = NULL;
}

//=============================================================================
FatFile::~FatFile ()
{
	flush();
	delete[] file;
}

//=============================================================================
bool FatFile::initialize (FAT *p, FatDirectory *d, int e, uint32_t c, uint32_t s)
{
	// 読み込み
	uint32_t bsize = p->getBytesPerSector();
	uint32_t sects = 0;
	uint8_t *ptr = NULL;
	last = 0;

	if (p->getNumberOfClusters() > c) {
		uint32_t size = ( s + bsize - 1 ) / bsize;

		ptr = p->readSectors(c, size, RESIZE_DELTA, &sects, &last);
		if (NULL == ptr)
			return false;
	}

	// 各種パラメータ確定
	fat = p;
	parent = d;
	file = ptr;
	flag = ptr + bsize * sects + sizeof(uint32_t) * sects;
	lba = (uint32_t*)( ptr + bsize * sects );
	fsize = s;
	sectors = sects;
	pos = 0;
	entry = e;
	sizeChanged = false;

	// フラグクリア
	clearFlag();

	return true;
}

//=============================================================================
uint32_t FatFile::read (uint8_t *bf, uint32_t sz)
{
	if (fsize < pos + sz)
		sz = fsize - pos;

	if (0 < sz) {
		memcpy(bf, file+pos, sz);
		pos += sz;
	}

	return sz;
}

//=============================================================================
uint32_t FatFile::write (uint8_t *bf, uint32_t sz)
{
	if (fsize < pos + sz) {
		uint32_t bytesPerSector = fat->getBytesPerSector();
		uint32_t size = bytesPerSector * sectors;

		if (size < pos + sz) {
			// クラスタの確保
			if (false == expandClusters(sz))
				return 0;
		}

		fsize = pos + sz;
		sizeChanged = true;
	}

	if (0 < sz) {
		memcpy(file+pos, bf, sz);

		// 書き込んだ位置にフラグを立てておく
		uint32_t bytesPerSector = fat->getBytesPerSector();
		uint32_t n = pos / bytesPerSector;
		while (n <= (pos + sz-1) / bytesPerSector) {
			flag[n++] = 1;
		}

		pos += sz;
	}

	return sz;
}

//=============================================================================
bool FatFile::seek (int pt, int flag)
{
	if (SEEK_SET == flag) {
		pos = pt;
		if (0 > pt)
			pos = 0;
	} else if (SEEK_CUR == flag) {
		pos += pt;
		if (0 > pt) {
			if (pos < (uint32_t)-pt)
				pos = 0;
		}
	} else if (SEEK_END == flag) {
		pos = fsize + pt;
		if (0 > pt) {
			if (fsize < (uint32_t)-pt)
				pos = 0;
		}
	}

	if (pos > fsize)
		pos = fsize;

	return true;
}

//=============================================================================
bool FatFile::flush ()
{
	// フラグが立っているセクタをディスクに書き戻す
	bool result = true;
	uint32_t bytesPerSector = fat->getBytesPerSector();

	for (uint32_t n = 0; n < sectors; n++) {
		if (0 != flag[n]) {
			result = fat->write(lba[n], file + n * bytesPerSector);
			flag[n] = 0;
		}
	}

	if (true == sizeChanged) {
		parent->setFileSize(entry, fsize);
		sizeChanged = false;
	}

	return result;
}

//=============================================================================
bool FatFile::resize (uint32_t sz)
{
	if (fsize == sz)
		return true;

	uint32_t bytesPerSector = fat->getBytesPerSector();
	uint32_t size = bytesPerSector * sectors;

	if (size < sz) {
		// クラスタを拡張する
		if (false == expandClusters(sz))
			return false;
	} else {
		// クラスタを縮小する
		reduceClusters(sz);
	}

	// ディレクトリエントリにファイルサイズを書き戻し
	parent->setFileSize(entry, sz);
	fsize = sz;

	return true;
}

//=============================================================================
uint32_t FatFile::position ()
{
	return pos;
}

//=============================================================================
uint32_t FatFile::size ()
{
	return fsize;
}

//-----------------------------------------------------------------------------
bool FatFile::expandClusters (uint32_t sz)
{
	uint32_t bsize = fat->getBytesPerSector();
	uint32_t size = bsize * sectors;
	uint32_t count = ( pos + sz - size + bsize - 1 ) / bsize;

	// クラスタの確保
	if (count < RESIZE_DELTA)
		count = RESIZE_DELTA;

	uint32_t cluster = fat->allocateCluster(last, count);
	if (0 == cluster)
		return false;

	if (0 < last)
		cluster = fat->getNextCluster(cluster);

	// メモリ確保
	uint32_t sects = sectors + count;
	size = bsize * sects;

	uint8_t *ptr = new uint8_t [ size + sizeof(uint32_t) * sects + sects ];
	if (NULL == ptr) {
		fat->freeCluster(cluster);
		if (0 < last)
			fat->setEndOfCluster(last);
		return false;
	}

	if (0 == last)
		parent->setCluster(entry, cluster);

	// ファイルデータをコピー
	memcpy(ptr, file, pos);
	memset(ptr + pos + sz, 0, size - pos - sz);

	// セクタ位置をコピー
	uint32_t *tmplba = (uint32_t*)( ptr + size );
	memcpy(tmplba, lba, sectors * sizeof(uint32_t));

	// フラグをコピー
	uint8_t *tmpflag = ptr + size + sizeof(uint32_t) * sects;
	memcpy(tmpflag, flag, sectors);

	for (uint32_t n = sectors; n < sects; n++)
		tmpflag[n] = 1;

	// セクタ位置を読み込む
	uint32_t num = fat->getSectorsPerCluster();
	uint32_t i = sectors;

	while (fat->getNumberOfClusters() > cluster) {
		uint32_t base = fat->getLbaFromCluster(cluster);

		for (uint32_t n = 0; n < num; n++)
			tmplba[i++] = base+n;

		last = cluster;
		cluster = fat->getNextCluster(cluster);
	}

	// 各種設定値更新
	delete file;

	file = ptr;
	flag = tmpflag;
	lba = tmplba;
	sectors = sects;

	return true;
}

//-----------------------------------------------------------------------------
void FatFile::reduceClusters (uint32_t sz)
{
	uint32_t bytesPerSector = fat->getBytesPerSector();
	uint32_t num = fat->getSectorsPerCluster();
	uint32_t bsize = bytesPerSector * num;
	uint32_t n = ( sz + bsize - 1 ) / bsize;

	if (sectors > n) {
		// セクタを開放する場合
		uint32_t cluster = fat->getClusterFromLba(lba[n]);
		fat->freeCluster(cluster);

		if (0 == n) {
			// ディレクトリエントリにクラスタを書き戻し
			parent->setCluster(entry, 0);
			last = 0;
		} else {
			// 幾つかクラスタを確保しているので、最後尾を設定
			last = fat->getClusterFromLba(lba[n-1]);
			fat->setEndOfCluster(last);
		}

		sectors = n;
	}

	if (pos > sz)
		pos = sz;
}

//-----------------------------------------------------------------------------
void FatFile::clearFlag ()
{
	for (uint32_t n = 0; n < sectors; n++) {
		flag[n] = 0;
	}
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// FatDirectory
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

//=============================================================================
FatDirectory::FatDirectory ()
{
	entrys = NULL;
}

//=============================================================================
FatDirectory::~FatDirectory ()
{
	delete[] entrys;
}

//=============================================================================
int FatDirectory::searchEntry (uint8_t *bf)
{
	// 与えられた名前を 8.3 形式にする
	uint8_t name[SIZE_FILENAME + SIZE_EXTENTION];
	expandFileName(name, bf);

	uint8_t *tmp = entrys;

	for ( int entry = 0 ; tmp < end ; entry++, tmp += 0x20 ) {
		if (MARK_DELETE == tmp[0] )
			continue;
		if (ATTR_VFAT == ( tmp[ATTRIBUTE] & 0x3f ))
			continue;
		if (MARK_UNUSED == tmp[0])
			break;

		// エントリの名前を比較する
		bool flag = false;
		int i;

		for (i = 0; i < SIZE_FILENAME + SIZE_EXTENTION; i++) {
			uint8_t c1 = name[i];
			uint8_t c2 = tmp[i];

			// SJIS の場合はエスケープする
			if (false == flag) {
				if ('A' <= c1 && 'Z' >= c1) {
					c1 |= 0x20;
				} else if (0x80 & c1)
					flag = true;
				if ('A' <= c2 && 'Z' >= c2)
					c2 |= 0x20;
			} else
				flag = false;
			if (c1 != c2)
			{
				break;
			}
		}

		if (SIZE_FILENAME + SIZE_EXTENTION == i)
			return entry;
	}

	return -1;
}

//=============================================================================
int FatDirectory::getEntryName (int entry, uint8_t *bf)
{
	uint8_t *tmp = entrys + 0x20 * entry;

	if (false == isValid(tmp))
		return -1;

	uint8_t name[SIZE_FILENAME + SIZE_EXTENTION + 2];
	int i, len = 0;

	// 必要な長さを計算するだけの場合
	if (NULL == bf)
		bf = name;

	// ファイル名をコピー
	for (i = 0; i < SIZE_FILENAME; i++) {
		if (' ' == tmp[i])
			break;
		bf[len++] = tmp[i];
	}

	if (' ' != tmp[SIZE_FILENAME]) {
		bf[len++] = '.';

		// 拡張子をコピー
		for (i = 0; i < SIZE_EXTENTION; i++) {
			if (' ' == tmp[SIZE_FILENAME + i])
				break;
			bf[len++] = tmp[SIZE_FILENAME + i];
		}
	}

	bf[len++] = '\0';

	return len;
}

//=============================================================================
int FatDirectory::getHeadEntry ()
{
	return getNextEntry(-1);
}

//=============================================================================
int FatDirectory::getNextEntry (int entry)
{
	entry++;

	if (0 > entry)
		entry = 0;

	// 次に有効なエントリを探す
	for (uint8_t *tmp = entrys + 0x20 * entry; tmp < end; tmp += 0x20, entry++) {
		if (MARK_DELETE == tmp[0] )
			continue;
		if (ATTR_VFAT == ( tmp[ATTRIBUTE] & 0x3f ))
			continue;
		if (MARK_UNUSED == tmp[0])
			break;

		return entry;
	}

	return -1;
}

//=============================================================================
bool FatDirectory::deleteEntry (int entry)
{
	uint8_t *tmp = entrys + 0x20 * entry;
	uint32_t cluster = little2host16(*((uint16_t*)( tmp + LOW_CLUSTER )));

	if (false == isValid(tmp))
		return false;

	if (ATTR_DIRECTORY & tmp[ATTRIBUTE]) {
		int i;

		// "." ".." は削除しない
		for (i = SIZE_FILENAME + SIZE_EXTENTION - 1; 0 <= i; i--)
			if (' ' != tmp[i])
				break;

		if (2 > i) {
			for ( ; 0 <= i; i--)
				if ('.' != tmp[i])
					break;
			if (0 > i)
				return true;
		}

		if (false == clearDirectory(entry))
			return false;
	}

	uint32_t bytesPerSector = fat->getBytesPerSector();
	uint32_t n = ( tmp - entrys ) / bytesPerSector;

	tmp[0] = MARK_DELETE;

	// VFAT を開放して回る
	for (tmp -= 0x20; tmp >= entrys; tmp -= 0x20) {
		if (ATTR_VFAT != ( tmp[ATTRIBUTE] & 0x3f ))
			break;

		if (n != (tmp - entrys ) / bytesPerSector) {
			// エントリをディスクに書き戻し
			fat->write(lba[n], entrys + n * bytesPerSector);
			n--;
		}

		tmp[ATTRIBUTE] = 0;

		if (tmp[0] & 0x40) {
			tmp[0] = MARK_DELETE;
			break;
		}
		tmp[0] = MARK_DELETE;
	}

	// エントリをディスクに書き戻し
	fat->write(lba[n], entrys + n * bytesPerSector);

	// FAT を解放
	fat->freeCluster(cluster);

	return true;
}

//=============================================================================
int FatDirectory::newDirectory (uint8_t *bf)
{
	// ディレクトリの埋め尽くしパターン
	uint32_t bsize = fat->getBytesPerSector();
	uint32_t num = fat->getSectorsPerCluster();

	uint8_t *sector = new uint8_t [bsize];

	// エントリの確保
	uint32_t sz = bsize * num;

	int entry = newEntry(bf, sz, ATTR_DIRECTORY, 0);
	if (-1 != entry) {
		uint8_t *tmp = entrys + 0x20 * entry;
		uint32_t cluster = little2host16(*((uint16_t*)( tmp + LOW_CLUSTER )));

		// "." と ".." エントリを作成する
		uint8_t name[SIZE_FILENAME + SIZE_EXTENTION];
		for (int i = 0; i < SIZE_FILENAME + SIZE_EXTENTION; i++)
			name[i] = ' ';

		memset(sector, 0, bsize);
		name[0] = '.';
		setEntry(sector, name, ATTR_DIRECTORY, cluster, 0);
		name[1] = '.';
		setEntry(sector+0x20, name, ATTR_DIRECTORY, start, 0);

		// 最初のセクタを書き込む
		uint32_t base = fat->getLbaFromCluster(cluster);

		fat->write(base, sector);

		// 続くセクタを書き込む
		memset(sector, 0, 0x20 * 2);
		for (uint32_t n = 1; n < num; n++)
			fat->write(base+n, sector);

		// クラスタを辿り全てのエントリを初期化
		cluster = fat->getNextCluster(cluster);

		while (cluster < fat->getNumberOfClusters()) {
			base = fat->getLbaFromCluster(cluster);
			num = fat->getSectorsPerCluster();

			for (uint32_t n = 0; n < num; n++)
				fat->write(base+n, sector);

			cluster = fat->getNextCluster(cluster);
		}
	}

	delete sector;

	return entry;
}

//=============================================================================
int FatDirectory::newFile (uint8_t *bf, uint32_t sz)
{
	return newEntry(bf, sz, 0, sz);
}

//=============================================================================
Directory* FatDirectory::getDirectory (int entry)
{
	uint8_t *tmp = entrys + 0x20 * entry;

	if (false == isValid(tmp))
		return NULL;

	if (0 == (tmp[ATTRIBUTE] & ATTR_DIRECTORY))
		return NULL;

	uint32_t cluster = little2host16(*((uint16_t*)( tmp + LOW_CLUSTER )));

	FatDirectory *dir;

	if (0 == cluster)
		dir = new FatRootDirectory();
	else
		dir = new FatSubDirectory();

	if (NULL == dir)
		return NULL;

	if (false == dir->initialize(fat, cluster)) {
		delete dir;
		return NULL;
	}

	return dir;
}

//=============================================================================
File* FatDirectory::getFile (int entry)
{
	uint8_t *tmp = entrys + 0x20 * entry;

	if (false == isValid(tmp))
		return NULL;

	if (tmp[ATTRIBUTE] & ATTR_DIRECTORY)
		return NULL;
	if (tmp[ATTRIBUTE] & ATTR_VOLUME)
		return NULL;

	uint32_t cluster = little2host16(*((uint16_t*)( tmp + LOW_CLUSTER )));
	if (0 == cluster)
		cluster = END_OF_CLUSTER;

	uint32_t size = little2host16(*((uint32_t*)( tmp + FILESIZE )));

	FatFile *file = new FatFile();
	if (NULL == file)
		return NULL;

	if (false == file->initialize(fat, this, entry, cluster, size)) {
		delete file;
		return NULL;
	}

	return file;
}

//=============================================================================
bool FatDirectory::isDirectory (int entry)
{
	uint8_t *tmp = entrys + 0x20 * entry;

	if (false == isValid(tmp))
		return false;

	if (0 == (tmp[ATTRIBUTE] & ATTR_DIRECTORY))
		return false;

	return true;
}

//=============================================================================
bool FatDirectory::isFile (int entry)
{
	uint8_t *tmp = entrys + 0x20 * entry;

	if (false == isValid(tmp))
		return false;

	if (tmp[ATTRIBUTE] & ATTR_DIRECTORY)
		return false;
	if (tmp[ATTRIBUTE] & ATTR_VOLUME)
		return false;

	return true;
}

//=============================================================================
uint32_t FatDirectory::getIdentifer ()
{
	return start;
}

//=============================================================================
bool FatDirectory::setFileSize (int entry, uint32_t size)
{
	uint8_t *tmp = entrys + 0x20 * entry;

	// ファイルサイズ
	*((uint32_t*)( tmp + FILESIZE )) = host2little32(size);

	// エントリをディスクに書き戻し
	uint32_t bytesPerSector = fat->getBytesPerSector();
	uint32_t n = ( tmp - entrys ) / bytesPerSector;
	fat->write(lba[n], entrys + n * bytesPerSector);

	return true;
}

//=============================================================================
bool FatDirectory::setCluster (int entry, uint32_t cluster)
{
	uint8_t *tmp = entrys + 0x20 * entry;

	// ファイルサイズ
	*((uint16_t*)( tmp + LOW_CLUSTER )) = host2little32(cluster);

	// エントリをディスクに書き戻し
	uint32_t bytesPerSector = fat->getBytesPerSector();
	uint32_t n = ( tmp - entrys ) / bytesPerSector;
	fat->write(lba[n], entrys + n * bytesPerSector);

	return true;
}

//-----------------------------------------------------------------------------
uint8_t* FatDirectory::searchUnusedEntry ()
{
	uint8_t *tmp = entrys;

	// 未使用エントリを探す
	while (tmp < end) {
		if (ATTR_VFAT != ( tmp[ATTRIBUTE] & 0x3f )) {
			if (MARK_UNUSED == tmp[0])
				break;
		}
		tmp += 0x20;
	}

	return tmp;
}

//-----------------------------------------------------------------------------
bool FatDirectory::isValid (uint8_t *ent)
{
	if (entrys > ent || end <= ent)
		return false;

	if (MARK_DELETE == ent[0] )
		return false;
	if (ATTR_VFAT == ( ent[ATTRIBUTE] & 0x3f ))
		return false;
	if (MARK_UNUSED == ent[0])
		return false;
	if (' ' == ent[0])
		return false;

	return true;
}

//-----------------------------------------------------------------------------
void FatDirectory::expandFileName (uint8_t *name, uint8_t *bf)
{
	int i, j, index = -1;

	for ( i = 0 ; '\0' != bf[i] ; i++ ){
		if( '.' == bf[i] )
			index = i;
	}

	int num = i;
	if (-1 != index)
		num = index;
	if (num > SIZE_FILENAME)
		num = SIZE_FILENAME;

	for (i = 0; i < num; i++)
		name[i] = bf[i];

	while (i < SIZE_FILENAME)
		name[i++] = ' ';

	if (-1 != index) {
		for (j = index+1; '\0' != bf[j]; j++) {
			if (i >= SIZE_FILENAME + SIZE_EXTENTION)
				break;
			name[i++] = bf[j];
		}
	}

	while (i < SIZE_FILENAME + SIZE_EXTENTION)
		name[i++] = ' ';

	//name[SIZE_FILENAME+SIZE_EXTENTION] = '\0';
}

//-----------------------------------------------------------------------------
bool FatDirectory::clearDirectory (int entry)
{
	bool result = true;

	Directory *dir = getDirectory(entry);
	if (NULL == dir)
		return false;

	entry = dir->getHeadEntry();
	while (-1 != entry) {
		if (false == dir->deleteEntry(entry))
			result = false;
		entry = dir->getNextEntry(entry);
	}

	delete dir;

	return result;
}

//-----------------------------------------------------------------------------
int FatDirectory::newEntry (uint8_t *bf, uint32_t sz, uint8_t attr, uint32_t fsize)
{
	// 名前があるか確認する
	if ('\0' == bf[0])
		return -1;

	// 空きエントリを探す
	int entry = searchFreeEntry();
	if (-1 == entry)
		return -1;

	uint32_t bsize = fat->getBytesPerSector() * fat->getSectorsPerCluster();
	uint32_t count = ( sz + bsize-1 ) / bsize;

	// クラスタの確保
	uint32_t cluster = 0;

	if (0 < count) {
		cluster = fat->allocateCluster(0, count);
		if (0 == cluster)
			return -1;
	}

	// 与えられた名前を 8.3 形式にする
	uint8_t name[SIZE_FILENAME + SIZE_EXTENTION];
	expandFileName(name, bf);

	// エントリの作成
	uint8_t *tmp = entrys + 0x20 * entry;
	setEntry(tmp, name, attr, cluster, fsize);

	// エントリをディスクに書き戻し
	uint32_t bytesPerSector = fat->getBytesPerSector();
	uint32_t n = ( tmp - entrys ) / bytesPerSector;
	fat->write(lba[n], entrys + n * bytesPerSector);

	return entry;
}

//-----------------------------------------------------------------------------
void FatDirectory::setEntry (uint8_t *ent, uint8_t *n, uint8_t a, uint16_t c, uint32_t s)
{
	// 名前
	memcpy(ent, n, SIZE_FILENAME + SIZE_EXTENTION);

	// 属性
	ent[ATTRIBUTE] = a;

	// 拡張領域
	ent[0x0c] = 0x00;
	ent[0x0d] = 0x00;
	ent[0x0e] = 0x00;
	ent[0x0f] = 0x00;
	ent[0x10] = 0x41;
	ent[0x11] = 0x00;
	ent[0x12] = 0x41;
	ent[0x13] = 0x00;
	ent[0x14] = 0x00;
	ent[0x15] = 0x00;
	ent[0x16] = 0x00;
	ent[0x17] = 0x00;
	ent[0x18] = 0x41;
	ent[0x19] = 0x00;

	// 開始クラスタ
	*((uint16_t*)( ent + LOW_CLUSTER )) = host2little16(c);

	// ファイルサイズ
	*((uint32_t*)( ent + FILESIZE )) = host2little32(s);

}

//-----------------------------------------------------------------------------
int FatDirectory::searchFreeEntry ()
{
	// 未使用エントリが無ければ削除エントリを使う
	uint8_t *tmp = entrys;
	while (tmp < unused) {
		if (MARK_DELETE == tmp[0])
			return ( tmp - entrys ) / 0x20;
		tmp += 0x20;
	}

	// 未使用エントリがあれば優先して使う
	if (tmp < end) {
		unused += 0x20;
		return ( tmp - entrys ) / 0x20;
	}

	// なければエントリを拡張する
	if (true == expandEntry()) {
		tmp = unused;
		unused += 0x20;
		return ( tmp - entrys ) / 0x20;
	}

	return -1;
}

//-----------------------------------------------------------------------------
bool FatDirectory::expandEntry ()
{
	if (0 == last)
		return false;

	uint32_t cluster = fat->allocateCluster(last, RESIZE_DELTA);
	if (0 == cluster)
		return false;

	cluster = fat->getNextCluster(cluster);

	uint32_t bsize = fat->getBytesPerSector();
	uint32_t sects = sectors + RESIZE_DELTA;
	uint32_t size = bsize * sects;

	uint8_t *ptr = new uint8_t [ size + sizeof(uint32_t) * sects + sects ];
	if (NULL == ptr) {
		fat->freeCluster(cluster);
		fat->setEndOfCluster(last);
		return false;
	}

	// エントリデータをコピー
	memcpy(ptr, entrys, bsize * sectors);
	memset(ptr + bsize * sectors, 0, bsize * (sects - sectors));

	// セクタ位置をコピー
	uint32_t *tmplba = (uint32_t*)( ptr + size );
	memcpy(tmplba, lba, sizeof(uint32_t) * sectors);

	// セクタ位置を読み込む
	uint32_t num = fat->getSectorsPerCluster();
	uint32_t i = sectors;

	while (fat->getNumberOfClusters() > cluster) {
		uint32_t base = fat->getLbaFromCluster(cluster);

		for (uint32_t n = 0; n < num; n++)
			tmplba[i++] = base+n;

		last = cluster;
		cluster = fat->getNextCluster(cluster);
	}

	delete entrys;

	// 各種パラメータ確定
	entrys = ptr;
	end = ptr + size;
	unused = ptr + bsize * sectors;
	lba = (uint32_t*)( ptr + size );
	sectors = sects;

	return true;
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// FatRootDirectory
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

//=============================================================================
bool FatRootDirectory::initialize (FAT *p, uint32_t c)
{
	uint32_t base = p->getRootDirectoryEntry();
	uint32_t num = p->getNumberOfDirEntry() / ( p->getBytesPerSector() / 0x20 );
	uint32_t sz = 0x20 * p->getNumberOfDirEntry();

	uint8_t *ent = new uint8_t [sz + num * sizeof(uint32_t)];
	if (NULL == ent)
		return false;

	// Root Directory の読み込み
	uint8_t *tmp = ent;
	uint32_t *tmplba = (uint32_t*)( ent + sz );

	for (uint32_t n = 0; n < num; n++) {
		if (false == p->read(base+n, tmp)) {
			delete[] ent;
			return false;
		}
		tmp += p->getBytesPerSector();
		tmplba[n] = base+n;
	}

	start = 0;
	last = 0;
	entrys = ent;
	end = tmp;
	lba = tmplba;
	fat = p;
	unused = searchUnusedEntry();

	return true;
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// FatSubDirectory
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

//=============================================================================
bool FatSubDirectory::initialize (FAT *p, uint32_t c)
{
	enum {
		DELTA = 224
	};

	// 読み込み
	uint32_t bsize = p->getBytesPerSector();
	uint32_t sects = 0;
	uint8_t *ptr = NULL;
	last = 0;

	if (p->getNumberOfClusters() > c) {
		ptr = p->readSectors(c, DELTA, DELTA, &sects, &last);
		if (NULL == ptr)
			return false;
	}

	// 各種パラメータ確定
	start = c;
	fat = p;
	entrys = ptr;
	end = ptr + bsize * sects;
	lba = (uint32_t*)( ptr + bsize * sects );
	sectors = sects;
	unused = searchUnusedEntry();

	return true;
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// FatStorage
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

//=============================================================================
FatStorage::FatStorage ()
{
	fat = NULL;
}

//=============================================================================
FatStorage::~FatStorage ()
{
	if (NULL != fat) delete fat;
}

//=============================================================================
bool FatStorage::initialize (IStorageDevice *p)
{
	fat = new FAT;
	if (NULL == fat)
		return false;

	if (false == fat->initialize(p)) {
		delete fat;
        fat = NULL;
		return false;
	}

	return true;
}

//=============================================================================
Directory* FatStorage::getRootDirectory ()
{
	FatRootDirectory *dir = new FatRootDirectory();
	if (NULL == dir)
		return NULL;

	if (false == dir->initialize(fat, 0)) {
		delete dir;
		return NULL;
	}

	return dir;
}
