#pragma once

class HDatabase
{
public:
	HDatabase(void);
	~HDatabase(void);

	_ConnectionPtr		m_pConnection;
	_RecordsetPtr		m_pRecordset;
	_variant_t			m_RecordsAffected;

	int OpenDatabase(CString strDatabaseName);
	int OpenTable(CString strTableName);
	int Find(CString strSql);
	int Add(CString strSql);
};
