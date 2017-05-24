#pragma once

namespace Fangame {

class IUserActionController;
class CBossDeathTable;
struct CBossInfo;
struct CBossAttackInfo;
//////////////////////////////////////////////////////////////////////////

class IMouseTarget {
public:
	virtual ~IMouseTarget() {}

	virtual void OnMouseMove() const = 0;
	virtual void OnMouseClick( IUserActionController& controller ) = 0;
	virtual void OnMouseDClick( IUserActionController& controller ) = 0;
};

//////////////////////////////////////////////////////////////////////////

class CAttackMouseTarget : public IMouseTarget {
public:
	explicit CAttackMouseTarget( CBossAttackInfo& attack, CBossDeathTable& _deathTable ) : targetAttack( attack ), deathTable( _deathTable ) {}

	virtual void OnMouseMove() const override final;
	virtual void OnMouseClick( IUserActionController& controller ) override final;
	virtual void OnMouseDClick( IUserActionController& controller ) override final;

private:
	CBossAttackInfo& targetAttack;
	CBossDeathTable& deathTable;

	void editAttack( IUserActionController& controller );
};

//////////////////////////////////////////////////////////////////////////

class CBossMouseTarget : public IMouseTarget {
public:
	explicit CBossMouseTarget( CBossInfo& boss, CBossDeathTable& _deathTable ) : targetBoss( boss ), deathTable( _deathTable ) {}

	virtual void OnMouseMove() const override final;
	virtual void OnMouseClick( IUserActionController& controller ) override final;
	virtual void OnMouseDClick( IUserActionController& controller ) override final;

private:
	CBossInfo& targetBoss;
	CBossDeathTable& deathTable;

	void editBoss( IUserActionController& controller );
};

//////////////////////////////////////////////////////////////////////////

class CFooterMouseTarget : public IMouseTarget {
public:
	explicit CFooterMouseTarget( CBossInfo& boss, CBossDeathTable& _deathTable ) : targetBoss( boss ), deathTable( _deathTable ) {}

	virtual void OnMouseMove() const override final;
	virtual void OnMouseClick( IUserActionController& controller ) override final;
	virtual void OnMouseDClick( IUserActionController& controller ) override final;

private:
	CBossInfo& targetBoss;
	CBossDeathTable& deathTable;

	void editBoss( IUserActionController& controller );
};

//////////////////////////////////////////////////////////////////////////

// Mouse target that invokes a given action.
class CIconMouseTarget : public IMouseTarget {
public:
	explicit CIconMouseTarget( CActionOwner<void( IUserActionController& )> _action ) : action( move( _action ) ) {}

	virtual void OnMouseMove() const override final;
	virtual void OnMouseClick( IUserActionController& controller ) override final;
	virtual void OnMouseDClick( IUserActionController& controller ) override final;

private:
	CActionOwner<void( IUserActionController& )> action;
};


//////////////////////////////////////////////////////////////////////////

}	// namespace Fangame.

