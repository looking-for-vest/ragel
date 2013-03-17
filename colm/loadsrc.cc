/*
 *  Copyright 2006-2012 Adrian Thurston <thurston@complang.org>
 */

/*  This file is part of Colm.
 *
 *  Colm is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 * 
 *  Colm is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with Colm; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 */

#include <iostream>
#include <errno.h>

#include "parser.h"
#include "config.h"
#include "avltree.h"
#include "parsedata.h"
#include "parser.h"
#include "global.h"
#include "input.h"
#include "loadsrc.h"
#include "exports2.h"
#include "colm/colm.h"

extern RuntimeData main_runtimeData;

NamespaceQual *LoadSource::walkRegionQual( region_qual regionQual )
{
	NamespaceQual *qual;
	if ( regionQual.RegionQual() != 0 ) {
		qual = walkRegionQual( regionQual.RegionQual() );
		qual->qualNames.append( String( regionQual.Id().text().c_str() ) );
	}
	else {
		qual = NamespaceQual::cons( namespaceStack.top() );
	}
	return qual;
}

RepeatType LoadSource::walkOptRepeat( opt_repeat OptRepeat )
{
	RepeatType repeatType = RepeatNone;
	if ( OptRepeat.Star() != 0 )
		repeatType = RepeatRepeat;
	else if ( OptRepeat.Plus() != 0 )
		repeatType = RepeatList;
	else if ( OptRepeat.Question() != 0 )
		repeatType = RepeatOpt;
	return repeatType;
}

TypeRef *LoadSource::walkTypeRef( type_ref &typeRefTree )
{
	NamespaceQual *nspaceQual = walkRegionQual( typeRefTree.RegionQual() );
	String id = typeRefTree.Id().text().c_str();
	RepeatType repeatType = walkOptRepeat( typeRefTree.OptRepeat() );

	return TypeRef::cons( internal, nspaceQual, id, repeatType );
}

void LoadSource::walkProdElList( ProdElList *list, prod_el_list &ProdElList )
{
	if ( ProdElList.ProdElList() != 0 ) {
		prod_el_list RightProdElList = ProdElList.ProdElList();
		walkProdElList( list, RightProdElList );
	}
	
	if ( ProdElList.ProdEl() != 0 ) {
		prod_el El = ProdElList.ProdEl();
		String typeName = El.Id().text().c_str();

		ObjectField *captureField = 0;
		if ( El.OptName().Name() != 0 ) {
			String fieldName = El.OptName().Name().text().c_str();
			captureField = ObjectField::cons( internal, 0, fieldName );
		}

		RepeatType repeatType = RepeatNone;
		if ( El.OptRepeat().Star() != 0 )
			repeatType = RepeatRepeat;

		ProdEl *prodEl = prodElName( internal, typeName,
				NamespaceQual::cons(namespaceStack.top()),
				captureField, repeatType, false );

		appendProdEl( list, prodEl );
	}
}

void LoadSource::walkProdList( LelDefList *lelDefList, prod_list &ProdList )
{
	if ( ProdList.ProdList() != 0 ) {
		prod_list RightProdList = ProdList.ProdList();
		walkProdList( lelDefList, RightProdList );
	}

	ProdElList *list = new ProdElList;
	
	prod_el_list ProdElList = ProdList.Prod().ProdElList();
	walkProdElList( list, ProdElList );

	Production *prod = BaseParser::production( internal, list, false, 0, 0 );
	prodAppend( lelDefList, prod );
}

LexFactor *LoadSource::walkLexFactor( lex_factor &LexFactorTree )
{
	if ( LexFactorTree.Literal() != 0 ) {
		String litString = LexFactorTree.Literal().text().c_str();
		Literal *literal = Literal::cons( internal, litString, Literal::LitString );
		LexFactor *factor = LexFactor::cons( literal );
		return factor;
	}
	else if ( LexFactorTree.Expr() != 0 ) {
		lex_expr LexExpr = LexFactorTree.Expr();
		LexExpression *expr = walkLexExpr( LexExpr );
		LexJoin *join = LexJoin::cons( expr );
		LexFactor *factor = LexFactor::cons( join );
		return factor;
	}
	else {
		String low = LexFactorTree.Low().text().c_str();
		Literal *lowLit = Literal::cons( internal, low, Literal::LitString );

		String high = LexFactorTree.High().text().c_str();
		Literal *highLit = Literal::cons( internal, high, Literal::LitString );

		Range *range = Range::cons( lowLit, highLit );
		LexFactor *factor = LexFactor::cons( range );
		return factor;
	}
}

LexFactorNeg *LoadSource::walkLexFactorNeg( lex_factor_neg &LexFactorNegTree )
{
	if ( LexFactorNegTree.FactorNeg() != 0 ) {
		lex_factor_neg Rec = LexFactorNegTree.FactorNeg();
		LexFactorNeg *recNeg = walkLexFactorNeg( Rec );
		LexFactorNeg *factorNeg = LexFactorNeg::cons( internal, recNeg, LexFactorNeg::CharNegateType );
		return factorNeg;
	}
	else {
		lex_factor LexFactorTree = LexFactorNegTree.Factor();
		LexFactor *factor = walkLexFactor( LexFactorTree );
		LexFactorNeg *factorNeg = LexFactorNeg::cons( internal, factor );
		return factorNeg;
	}
}

LexFactorRep *LoadSource::walkLexFactorRep( lex_factor_rep &LexFactorRepTree )
{
	if ( LexFactorRepTree.FactorRep() != 0 ) {
		lex_factor_rep Rec = LexFactorRepTree.FactorRep();
		LexFactorRep *recRep = walkLexFactorRep( Rec );
		LexFactorRep *factorRep = LexFactorRep::cons( internal, recRep, 0, 0, LexFactorRep::StarType );
		return factorRep;
	}
	else {
		lex_factor_neg LexFactorNegTree = LexFactorRepTree.FactorNeg();
		LexFactorNeg *factorNeg = walkLexFactorNeg( LexFactorNegTree );
		LexFactorRep *factorRep = LexFactorRep::cons( internal, factorNeg );
		return factorRep;
	}
}

LexFactorAug *LoadSource::walkLexFactorAug( lex_factor_rep &LexFactorRepTree )
{
	LexFactorRep *factorRep = walkLexFactorRep( LexFactorRepTree );
	return LexFactorAug::cons( factorRep );
}

LexTerm *LoadSource::walkLexTerm( lex_term &LexTermTree )
{
	if ( LexTermTree.Term() != 0 ) {
		lex_term Rec = LexTermTree.Term();
		LexTerm *leftTerm = walkLexTerm( Rec );

		lex_factor_rep LexFactorRepTree = LexTermTree.FactorRep();
		LexFactorAug *factorAug = walkLexFactorAug( LexFactorRepTree );
		LexTerm *term = LexTerm::cons( leftTerm, factorAug, LexTerm::ConcatType );
		return term;
	}
	else {
		lex_factor_rep LexFactorRepTree = LexTermTree.FactorRep();
		LexFactorAug *factorAug = walkLexFactorAug( LexFactorRepTree );
		LexTerm *term = LexTerm::cons( factorAug );
		return term;
	}
}

LexExpression *LoadSource::walkLexExpr( lex_expr &LexExprTree )
{
	if ( LexExprTree.Expr() != 0 ) {
		lex_expr Rec = LexExprTree.Expr();
		LexExpression *leftExpr = walkLexExpr( Rec );

		lex_term LexTermTree = LexExprTree.Term();
		LexTerm *term = walkLexTerm( LexTermTree );
		LexExpression *expr = LexExpression::cons( leftExpr, term, LexExpression::OrType );

		return expr;
	}
	else {
		lex_term LexTermTree = LexExprTree.Term();
		LexTerm *term = walkLexTerm( LexTermTree );
		LexExpression *expr = LexExpression::cons( term );
		return expr;
	}
}

void LoadSource::walkTokenList( token_list &TokenList )
{
	if ( TokenList.TokenList() != 0 ) {
		token_list RightTokenList = TokenList.TokenList();
		walkTokenList( RightTokenList );
	}
	
	if ( TokenList.TokenDef() != 0 ) {
		token_def TokenDef = TokenList.TokenDef();
		String name = TokenDef.Id().text().c_str();

		ObjectDef *objectDef = ObjectDef::cons( ObjectDef::UserType, name, pd->nextObjectId++ ); 

		lex_expr LexExpr = TokenDef.Expr();
		LexExpression *expr = walkLexExpr( LexExpr );
		LexJoin *join = LexJoin::cons( expr );

		defineToken( internal, name, join, objectDef, 0, false, false, false );
	}

	if ( TokenList.IgnoreDef() != 0 ) {
		ignore_def IgnoreDef = TokenList.IgnoreDef();

		ObjectDef *objectDef = ObjectDef::cons( ObjectDef::UserType, 0, pd->nextObjectId++ ); 

		lex_expr LexExpr = IgnoreDef.Expr();
		LexExpression *expr = walkLexExpr( LexExpr );
		LexJoin *join = LexJoin::cons( expr );

		defineToken( internal, 0, join, objectDef, 0, true, false, false );
	}
}

void LoadSource::walkLexRegion( region_def &regionDef )
{
	pushRegionSet( internal );

	token_list TokenList = regionDef.TokenList();
	walkTokenList( TokenList );

	popRegionSet();
}

void LoadSource::walkCflDef( cfl_def &cflDef )
{
	prod_list prodList = cflDef.ProdList();

	LelDefList *defList = new LelDefList;
	walkProdList( defList, prodList );

	String name = cflDef.DefId().text().c_str();
	NtDef *ntDef = NtDef::cons( name, namespaceStack.top(), contextStack.top(), false );
	ObjectDef *objectDef = ObjectDef::cons( ObjectDef::UserType, name, pd->nextObjectId++ ); 
	BaseParser::cflDef( ntDef, objectDef, defList );
}

ExprVect *LoadSource::walkCodeExprList( _repeat_code_expr &codeExprList )
{
	ExprVect *exprVect = new ExprVect;
	while ( !codeExprList.end() ) {
		code_expr codeExpr = codeExprList.value();
		LangExpr *expr = walkCodeExpr( codeExpr );
		exprVect->append( expr );
		codeExprList = codeExprList.next();
	}
	return exprVect;
}

LangStmt *LoadSource::walkPrintStmt( print_stmt &printStmt )
{
	//std::cerr << "print statement: " << printStmt.text() << std::endl;

	_repeat_code_expr codeExprList = printStmt.CodeExprList();
	ExprVect *exprVect = walkCodeExprList( codeExprList );
	return LangStmt::cons( internal, LangStmt::PrintType, exprVect );
}

QualItemVect *LoadSource::walkQual( qual &Qual )
{
	QualItemVect *qualItemVect;
	qual RecQual = Qual.Qual();
	if ( RecQual != 0 ) {
		qualItemVect = walkQual( RecQual );
		String id = Qual.Id().text().c_str();
		QualItem::Type type = Qual.Dot() != 0 ? QualItem::Dot : QualItem::Arrow;
		qualItemVect->append( QualItem( internal, id, type ) );
	}
	else {
		qualItemVect = new QualItemVect;
	}
	return qualItemVect;
}

LangVarRef *LoadSource::walkVarRef( var_ref &varRef )
{
	qual Qual = varRef.Qual();
	QualItemVect *qualItemVect = walkQual( Qual );
	String id = varRef.Id().text().c_str();
	LangVarRef *langVarRef = LangVarRef::cons( internal, qualItemVect, id );
	return langVarRef;
}

LangExpr *LoadSource::walkCodeExpr( code_expr &codeExpr )
{
	LangExpr *expr = 0;
	if ( codeExpr.VarRef() != 0 ) {
		var_ref varRef = codeExpr.VarRef();
		LangVarRef *langVarRef = walkVarRef( varRef );
		LangTerm *term;
		if ( codeExpr.CodeExprList() == 0 ) {
			term = LangTerm::cons( internal, LangTerm::VarRefType, langVarRef );
		}
		else {
			_repeat_code_expr codeExprList = codeExpr.CodeExprList();
			ExprVect *exprVect = walkCodeExprList( codeExprList );
			term = LangTerm::cons( internal, langVarRef, exprVect );
		}

		expr = LangExpr::cons( term );
	}
	else if ( codeExpr.Number() != 0 ) {
		String number = codeExpr.Lit().text().c_str();
		LangTerm *term =LangTerm::cons( InputLoc(), LangTerm::NumberType, number );
		expr = LangExpr::cons( term );
	}
	else if ( codeExpr.Lit() != 0 ) {
		String lit = codeExpr.Lit().text().c_str();
		LangTerm *term = LangTerm::cons( internal, LangTerm::StringType, lit );
		expr = LangExpr::cons( term );
	}
	else {
		/* The type we are parsing. */
		type_ref typeRefTree = codeExpr.TypeRef();
		TypeRef *typeRef = walkTypeRef( typeRefTree );

		LangVarRef *varRef = LangVarRef::cons( internal, new QualItemVect, String("stdin") );
		LangExpr *accumExpr = LangExpr::cons( LangTerm::cons( internal, LangTerm::VarRefType, varRef ) );

		ConsItem *consItem = ConsItem::cons( internal, ConsItem::ExprType, accumExpr );
		ConsItemList *list = ConsItemList::cons( consItem );

		ObjectField *objField = ObjectField::cons( internal, 0, String("P") );

		expr = parseCmd( internal, false, objField, typeRef, 0, list );
	}
	return expr;
}

LangStmt *LoadSource::walkExprStmt( expr_stmt &exprStmt )
{
	LangStmt *stmt;
	if ( exprStmt.CodeExpr() != 0 ) {
		code_expr codeExpr = exprStmt.CodeExpr();
		LangExpr *expr = walkCodeExpr( codeExpr );
		stmt = LangStmt::cons( internal, LangStmt::ExprType, expr );
	}
	return stmt;
}

LangStmt *LoadSource::walkStatement( statement &Statement )
{
	LangStmt *stmt;
	if ( Statement.Print() != 0 ) {
		print_stmt printStmt = Statement.Print();
		stmt = walkPrintStmt( printStmt );
	}
	else if ( Statement.Expr() != 0 ) {
		expr_stmt exprStmt = Statement.Expr();
		stmt = walkExprStmt( exprStmt );
	}
	return stmt;
}

void LoadSource::walkNamespaceDef( namespace_def NamespaceDef )
{
	String name = NamespaceDef.Name().text().c_str();
	createNamespace( internal, name );
	walkRootItemList( NamespaceDef.RootItemList() );
	namespaceStack.pop();
}

void LoadSource::walkRootItem( root_item &rootItem, StmtList *stmtList )
{
	if ( rootItem.CflDef() != 0 ) {
		cfl_def cflDef = rootItem.CflDef();
		walkCflDef( cflDef );
	}
	else if ( rootItem.RegionDef() != 0 ) {
		region_def regionDef = rootItem.RegionDef();
		walkLexRegion( regionDef );
	}
	else if ( rootItem.Statement() != 0 ) {
		statement Statement = rootItem.Statement();
		LangStmt *stmt = walkStatement( Statement );
		stmtList->append( stmt );
	}
	else if ( rootItem.NamespaceDef() != 0 ) {
		walkNamespaceDef( rootItem.NamespaceDef() );
	}
}

StmtList *LoadSource::walkRootItemList( _repeat_root_item rootItemList )
{
	StmtList *stmtList = new StmtList;

	/* Walk the list of items. */
	while ( !rootItemList.end() ) {
		root_item rootItem = rootItemList.value();
		walkRootItem( rootItem, stmtList );
		rootItemList = rootItemList.next();
	}
	return stmtList;
}

void LoadSource::go()
{
	const char *argv[2];
	argv[0] = inputFileName;
	argv[1] = 0;

	colmInit( 0 );
	ColmProgram *program = colmNewProgram( &main_runtimeData );
	colmRunProgram( program, 1, argv );

	/* Extract the parse tree. */
	start Start = ColmTree( program );

	if ( Start == 0 ) {
		gblErrorCount += 1;
		std::cerr << inputFileName << ": parse error" << std::endl;
		return;
	}

	StmtList *stmtList = walkRootItemList( Start.RootItemList() );
	colmDeleteProgram( program );

	pd->rootCodeBlock = CodeBlock::cons( stmtList, 0 );
}

