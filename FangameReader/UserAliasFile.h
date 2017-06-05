#pragma once

namespace Fangame {

enum TAttackCurrentStatus;
//////////////////////////////////////////////////////////////////////////

// File with user defined aliases for boss and attack names and layout parameters.
class CUserAliasFile {
public:
	explicit CUserAliasFile( CUnicodeView fileName );

	void SaveChanges();

	CUnicodePart GetUserBossName( CUnicodePart baseName ) const;
	TAttackCurrentStatus GetUserBossStatus( CUnicodePart bossName, TAttackCurrentStatus baseStatus ) const;
	CUnicodePart GetUserAttackName( CUnicodePart bossName, CUnicodePart baseAttackName, CUnicodePart attackDefaultName ) const;
	CUnicodePart GetUserIconPath( CUnicodePart bossName, CUnicodePart attackName, CUnicodePart defaultPath ) const;
	TAttackCurrentStatus GetUserAttackStatus( CUnicodePart bossName, CUnicodePart attackName, TAttackCurrentStatus baseStatus ) const;

	void SetUserBossName( CUnicodePart baseName, CUnicodePart newName );
	void SetUserBossStatus( CUnicodePart baseName, TAttackCurrentStatus newStatus );
	void SetUserAttackName( CUnicodePart bossName, CUnicodePart baseAttackName, CUnicodePart newAttackName );
	void SetUserIconPath( CUnicodePart bossName, CUnicodePart baseAttackName, CUnicodePart newPath );
	void SetUserAttackStatus( CUnicodePart bossName, CUnicodePart baseAttackName, TAttackCurrentStatus newStatus );

private:
	struct CAttackAliasData {
		CXmlElement& SrcElem;
		CUnicodeString Name;
		CUnicodeString IconPath;
		TAttackCurrentStatus Status;

		explicit CAttackAliasData( CXmlElement& src ) : SrcElem( src ) {}
	};

	struct CBossAliasData {
		CXmlElement& SrcElem;
		CUnicodeString Name;
		TAttackCurrentStatus Status;
		CMap<CUnicodeString, CAttackAliasData> Attacks;

		explicit CBossAliasData( CXmlElement& src ) : SrcElem( src ) {}
	};
	
	CXmlDocument aliasDoc;
	CMap<CUnicodeString, CBossAliasData> bossAliases;

	void parseAliasElement( CXmlElement& elem );
	void parseAttackAlias( CXmlElement& elem, CBossAliasData& parent );

	CBossAliasData& createBossData( CUnicodePart name );
	CAttackAliasData& createAttackData( CBossAliasData& parent, CUnicodePart name );
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

