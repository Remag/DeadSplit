#include <common.h>
#pragma hdrstop

#include <UserAliasFile.h>
#include <BossInfo.h>

namespace Fangame {

//////////////////////////////////////////////////////////////////////////

const CUnicodeView aliasRootName = L"Aliases";
CUserAliasFile::CUserAliasFile( CUnicodeView fileName )
{
	try {
		aliasDoc.LoadFromFile( fileName );
	} catch( CException& ) {
		// File doesn't exists, it's ok.
		aliasDoc.SetRoot( aliasRootName );
		return;
	}

	for( auto& child : aliasDoc.GetRoot().Children() ) {
		parseAliasElement( child );
	}
}

void CUserAliasFile::SaveChanges()
{
	aliasDoc.SaveToFile( aliasDoc.GetName() );
}

const CUnicodeView keyNameAttrib = L"name";
const CUnicodeView nameAliasAttrib = L"nameAlias";
const CUnicodeView statusAttrib = L"statusAlias";
void CUserAliasFile::parseAliasElement( CXmlElement& elem )
{
	const auto name = elem.GetAttributeValue( keyNameAttrib, CUnicodePart() );
	auto& bossData = bossAliases.GetOrCreate( name, elem ).Value();
	bossData.Name = elem.GetAttributeValue( nameAliasAttrib, CUnicodePart() );
	const auto statusStr = elem.GetAttributeValue( statusAttrib, CUnicodePart() );
	bossData.Status = AttackStatusToNameDict.FindEnum( statusStr, ACS_EnumCount );
	for( auto& child : elem.Children() ) {
		parseAttackAlias( child, bossData );
	}
}

const CUnicodeView iconAttrib = L"iconAlias";
void CUserAliasFile::parseAttackAlias( CXmlElement& elem, CBossAliasData& parent )
{
	const auto name = elem.GetAttributeValue( keyNameAttrib, CUnicodePart() );
	auto& attackData = parent.Attacks.GetOrCreate( name, elem ).Value();
	attackData.Name = elem.GetAttributeValue( nameAliasAttrib, CUnicodePart() );
	attackData.IconPath = elem.GetAttributeValue( iconAttrib, CUnicodePart() );
	const auto statusStr = elem.GetAttributeValue( statusAttrib, CUnicodePart() );
	attackData.Status = AttackStatusToNameDict.FindEnum( statusStr, ACS_EnumCount );
}

CUnicodePart CUserAliasFile::GetUserBossName( CUnicodePart baseName ) const
{
	const auto bossData = bossAliases.Get( baseName );
	if( bossData == nullptr ) {
		return baseName;
	}
	return bossData->Name.IsEmpty() ? baseName : bossData->Name;
}

TAttackCurrentStatus CUserAliasFile::GetUserBossStatus( CUnicodePart bossName, TAttackCurrentStatus baseStatus ) const
{
	const auto bossData = bossAliases.Get( bossName );
	if( bossData == nullptr ) {
		return baseStatus;
	}
	return bossData->Status == ACS_EnumCount ? baseStatus : bossData->Status;
}

CUnicodePart CUserAliasFile::GetUserAttackName( CUnicodePart bossName, CUnicodePart baseAttackName, CUnicodePart attackDefaultName ) const
{
	const auto bossData = bossAliases.Get( bossName );
	if( bossData == nullptr ) {
		return attackDefaultName;
	}
	const auto attackData = bossData->Attacks.Get( baseAttackName );
	if( attackData == nullptr ) {
		return attackDefaultName;
	}

	return attackData->Name.IsEmpty() ? attackDefaultName : attackData->Name;
}

CUnicodePart CUserAliasFile::GetUserIconPath( CUnicodePart bossName, CUnicodePart attackName, CUnicodePart defaultPath ) const
{
	const auto bossData = bossAliases.Get( bossName );
	if( bossData == nullptr ) {
		return defaultPath;
	}
	const auto attackData = bossData->Attacks.Get( attackName );
	if( attackData == nullptr ) {
		return defaultPath;
	}

	return attackData->IconPath.IsEmpty() ? defaultPath : attackData->IconPath;
}

TAttackCurrentStatus CUserAliasFile::GetUserAttackStatus( CUnicodePart bossName, CUnicodePart attackName, TAttackCurrentStatus baseStatus ) const
{
	const auto bossData = bossAliases.Get( bossName );
	if( bossData == nullptr ) {
		return baseStatus;
	}
	const auto attackData = bossData->Attacks.Get( attackName );
	if( attackData == nullptr ) {
		return baseStatus;
	}

	return attackData->Status == ACS_EnumCount ? baseStatus : attackData->Status;
}

void CUserAliasFile::SetUserBossName( CUnicodePart baseName, CUnicodePart newName )
{
	auto bossData = bossAliases.Get( baseName );
	if( bossData == nullptr ) {
		bossData = &createBossData( baseName );
	}
	
	assert( bossData != nullptr );
	bossData->Name = newName;
	bossData->SrcElem.SetAttributeValue( nameAliasAttrib, newName );
}

void CUserAliasFile::SetUserBossStatus( CUnicodePart baseName, TAttackCurrentStatus newStatus ) 
{
	auto bossData = bossAliases.Get( baseName );
	if( bossData == nullptr ) {
		bossData = &createBossData( baseName );
	}
	
	assert( bossData != nullptr );
	bossData->Status = newStatus;
	bossData->SrcElem.SetAttributeValue( statusAttrib, AttackStatusToNameDict[newStatus] );
}

void CUserAliasFile::SetUserAttackName( CUnicodePart bossName, CUnicodePart baseAttackName, CUnicodePart newAttackName )
{
	auto bossData = bossAliases.Get( bossName );
	if( bossData == nullptr ) {
		bossData = &createBossData( bossName );
	}
	
	assert( bossData != nullptr );
	auto attackData = bossData->Attacks.Get( baseAttackName );
	if( attackData == nullptr ) {
		attackData = &createAttackData( *bossData, baseAttackName );
	}
	assert( attackData != nullptr );
	attackData->Name = newAttackName;
	attackData->SrcElem.SetAttributeValue( nameAliasAttrib, newAttackName );
}

void CUserAliasFile::SetUserIconPath( CUnicodePart bossName, CUnicodePart baseAttackName, CUnicodePart newPath )
{
	auto bossData = bossAliases.Get( bossName );
	if( bossData == nullptr ) {
		bossData = &createBossData( bossName );
	}
	
	assert( bossData != nullptr );
	auto attackData = bossData->Attacks.Get( baseAttackName );
	if( attackData == nullptr ) {
		attackData = &createAttackData( *bossData, baseAttackName );
	}
	assert( attackData != nullptr );
	attackData->IconPath = newPath;
	attackData->SrcElem.SetAttributeValue( iconAttrib, newPath );
}

void CUserAliasFile::SetUserAttackStatus( CUnicodePart bossName, CUnicodePart baseAttackName, TAttackCurrentStatus newStatus )
{
	auto bossData = bossAliases.Get( bossName );
	if( bossData == nullptr ) {
		bossData = &createBossData( bossName );
	}
	
	assert( bossData != nullptr );
		auto attackData = bossData->Attacks.Get( baseAttackName );
	if( attackData == nullptr ) {
	attackData = &createAttackData( *bossData, baseAttackName );
	}
	assert( attackData != nullptr );
	attackData->Status = newStatus;
	attackData->SrcElem.SetAttributeValue( statusAttrib, AttackStatusToNameDict[newStatus] );
}

const CUnicodeView bossChildName = L"Boss";
CUserAliasFile::CBossAliasData& CUserAliasFile::createBossData( CUnicodePart name )
{
	auto& root = aliasDoc.GetRoot();
	auto& newElem = root.CreateChild( bossChildName );
	newElem.AddAttribute( keyNameAttrib, name );
	return bossAliases.Add( name, newElem ).Value();
}

const CUnicodeView attackChildName = L"Attack";
CUserAliasFile::CAttackAliasData& CUserAliasFile::createAttackData( CBossAliasData& parent, CUnicodePart name )
{
	auto& parentElem = parent.SrcElem;
	auto& newElem = parentElem.CreateChild( attackChildName );
	newElem.AddAttribute( keyNameAttrib, name );
	return parent.Attacks.Add( name, newElem ).Value();
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.
