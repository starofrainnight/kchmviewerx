/***************************************************************************
 *   Copyright (C) 2004-2005 by Georgy Yunaev, gyunaev@ulduzsoft.com       *
 *   Please do not use email address above for bug reports; see            *
 *   the README file                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <qtoolbutton.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qtextedit.h>
#include <qaccel.h>
#include <qpopupmenu.h>
#include <qmenubar.h>
 
#include "libchmfile.h"
#include "libchmfileimpl.h"

#include "kchmmainwindow.h"
#include "kchmviewwindow.h"
#include "kchmconfig.h"
#include "kchmsearchtoolbar.h"
#include "kqrunprocess.h"
#include "kchmtreeviewitem.h"
#include "kchmcontentswindow.h"
#include "iconstorage.h"

#include "kchmsearchtoolbar.moc"

static KQPopupMenu *menu_langlist, *menu_enclist;

KCHMSearchAndViewToolbar::KCHMSearchAndViewToolbar( KCHMMainWindow * parent )
	: QToolBar (parent)
{
	// Toolbar label
	setLabel( i18n( "Find in page") );

	// Load the pixmaps
    QPixmap iconPrev (*gIconStorage.getToolbarPixmap(KCHMIconStorage::findprev));
    QPixmap iconNext (*gIconStorage.getToolbarPixmap(KCHMIconStorage::findnext));
    QPixmap iconFontInc (*gIconStorage.getToolbarPixmap(KCHMIconStorage::view_increase));
    QPixmap iconFontDec (*gIconStorage.getToolbarPixmap(KCHMIconStorage::view_decrease));
    QPixmap iconViewSource (*gIconStorage.getToolbarPixmap(KCHMIconStorage::viewsource));
    QPixmap iconAddBookmark (*gIconStorage.getToolbarPixmap(KCHMIconStorage::bookmark_add));
	QPixmap iconNextPage (*gIconStorage.getToolbarPixmap(KCHMIconStorage::next_page));
	QPixmap iconPrevPage (*gIconStorage.getToolbarPixmap(KCHMIconStorage::prev_page));
	QPixmap iconLocateInContent (*gIconStorage.getToolbarPixmap(KCHMIconStorage::locate_in_content));

	// Create the combobox to enter the find text
	m_findBox = new QComboBox (TRUE, this);
	m_findBox->setMinimumWidth (200);
	connect( m_findBox->lineEdit(), SIGNAL( returnPressed() ), this, SLOT( onReturnPressed() ) );
	QWhatsThis::add( m_findBox, i18n( "Enter here the text to search in the current page.") );	
	
	QAccel *acc = new QAccel( this );
	acc->connectItem( acc->insertItem(Key_F+CTRL), this, SLOT( onAccelFocusSearchField() ) );
	
	// Button 'prevous search result'
	m_buttonPrev = new QToolButton (iconPrev,
				i18n( "Previous search result"),
				QString::null,
				this,
				SLOT(onBtnPrevSearchResult()),
				this);
	QWhatsThis::add( m_buttonPrev, i18n( "Click this button to find previous search result.") );

	// Button 'next search result'
	m_buttonNext = new QToolButton (iconNext,
				i18n( "Next search result"),
				QString::null,
				this,
				SLOT(onBtnNextSearchResult()),
				this);
	QWhatsThis::add( m_buttonNext, i18n( "Click this button to find next search result.") );

	// Button 'locate in content'
	m_buttonLocateInContent = new QToolButton( iconLocateInContent,
				i18n( "Locate this topic in content window"),
				QString::null,
				this,
				SLOT( onBtnLocateInContentWindow() ),
				this );
	QWhatsThis::add( m_buttonLocateInContent, i18n( "Click this button to find current topic in the content window, and open it.") );
	
	// Button 'increase font size'
	m_buttonFontInc = new QToolButton (iconFontInc,
				i18n( "Increase font size"),
				QString::null,
				this,
				SLOT(onBtnFontInc()),
				this);
	QWhatsThis::add( m_buttonFontInc, i18n( "Click this button to increase the font size.") );

	// Button 'decrease font size'
	m_buttonFontDec = new QToolButton (iconFontDec,
				i18n( "Decrease font size"),
				QString::null,
				this,
				SLOT(onBtnFontDec()),
				this);
	QWhatsThis::add( m_buttonFontDec, i18n( "Click this button to decrease the font size.") );
	
	// Button 'view HTML source'
	m_buttonViewSource = new QToolButton (iconViewSource,
				i18n( "View HTML source"),
				QString::null,
				this,
				SLOT(onBtnViewSource()),
				this);
	QWhatsThis::add( m_buttonViewSource, i18n( "Click this button to open a separate window with the page HTML source.") );
	
	// Button 'add a bookmark'
	m_buttonAddBookmark = new QToolButton (iconAddBookmark,
				i18n( "Add to bookmarks"),
				QString::null,
				this,
				SLOT(onBtnAddBookmark()),
				this);
	QWhatsThis::add( m_buttonAddBookmark, i18n( "Click this button to add the current page to the bookmarks list.") );
	
	m_buttonPrevPageInTOC = new QToolButton( iconPrevPage,
				i18n( "Prev page in TOC"),
				QString::null,
				this,
				SLOT(onBtnPrevPageInToc()),
				this);
	QWhatsThis::add( m_buttonAddBookmark, i18n( "Click this button to go to previous page in Table Of Content.") );
	
	m_buttonNextPageInTOC = new QToolButton (iconNextPage,
				i18n( "Next page in TOC"),
				QString::null,
				this,
				SLOT(onBtnNextPageInToc()),
				this);
	QWhatsThis::add( m_buttonAddBookmark, i18n( "Click this button to go to next page in Table of Content.") );
	
	// Create the approptiate menu entries in 'View' main menu
	m_MenuView = new KQPopupMenu( parent );
	parent->menuBar()->insertItem( i18n( "&View"), m_MenuView );

	m_MenuView->insertItem( i18n( "&Increase font"), this, SLOT(onBtnFontInc()), CTRL+Key_Plus );
	m_MenuView->insertItem( i18n( "&Decrease font"), this, SLOT(onBtnFontDec()), CTRL+Key_Minus );
	m_MenuView->insertItem( i18n( "&View HTML source"), this, SLOT(onBtnViewSource()), CTRL+Key_U );
	
    m_MenuView->insertSeparator();
	m_MenuView->insertItem( i18n( "&Bookmark this page"), this, SLOT(onBtnAddBookmark()), CTRL+Key_T  );
    m_MenuView->insertSeparator();
	
	m_menuShowFullscreenMenuID = m_MenuView->insertItem( i18n( "&Full screen"), this,
			 SLOT(onBtnFullScreen()), Key_F11  );
	m_menuShowContentWindowMenuID = m_MenuView->insertItem( i18n( "&Show contents window"), this,
			 SLOT(onBtnToggleContentWindow()), Key_F9 );
	m_MenuView->insertItem( i18n( "&Locate in contents window"), this,
			 SLOT(onBtnLocateInContentWindow()), CTRL+Key_L  );
	
	m_MenuView->setItemChecked( m_menuShowFullscreenMenuID, false ); 
	m_MenuView->setItemChecked( m_menuShowContentWindowMenuID, true ); 
	m_MenuView->insertSeparator();
		
	// Create the language selection menu.
    menu_langlist = new KQPopupMenu( parent );
	KQPopupMenu * menu_sublang = 0;

	// Because the encoding menu is very large, it is not reasonable to have a slot for every item.
	// It is simplier just to use a single slot for any menu item of this submenu.
	connect (menu_langlist, SIGNAL( activated(int) ), this, SLOT ( onMenuActivated(int) ));
	
	// Add the language entries
	const LCHMTextEncoding * enctable = LCHMFileImpl::getTextEncodingTable();
	int idx;
			
	for ( idx = 0; (enctable + idx)->language; idx++ )
	{
		// See the next item; does is have the same charset as current?
		const LCHMTextEncoding * item = enctable + idx;
		const LCHMTextEncoding * nextitem = enctable + idx + 1;
		
		if ( nextitem->language
		&& !strcmp( item->language, nextitem->language) )
		{
			// If charset is the same as next one, create a new popup menu.
			// If the menu is already created, add to it
			if ( !menu_sublang )
			{
				menu_sublang = new KQPopupMenu( menu_langlist );
				connect (menu_sublang, SIGNAL( activated(int) ), this, SLOT ( onMenuActivated(int) ));
			}
				
			menu_sublang->insertItem( item->sublanguage, idx );
			continue;
		}
		
		// If the next charset differs from this one,
		// add a submenu if menu_sublang is already created.
		// otherwise, just add an item
		if ( menu_sublang )
		{
			menu_sublang->insertItem( item->sublanguage, idx );
			menu_langlist->insertItem( item->language, menu_sublang );
			menu_sublang = 0;
		}
		else
			menu_langlist->insertItem( item->language, idx );
	}

	m_MenuView->insertItem( i18n( "&Set language"), menu_langlist );
	m_checkedEncodingInMenu = -1;
	m_checkedLanguageInMenu = -1;

	// Special menu for very smart people just to select codepage
	QMap<QString,bool> addedCharsets;
	menu_enclist = new KQPopupMenu( parent );

	connect (menu_enclist, SIGNAL( activated(int) ), this, SLOT ( onMenuActivated(int) ));
	
	// Add the codepage entries
	for ( idx = 0; (enctable + idx)->language; idx++ )
	{
		const LCHMTextEncoding * item = enctable + idx;
		
		// This menu is only for charsets, so we won't add duplicate charset twice
		if ( addedCharsets.find( item->qtcodec ) != addedCharsets.end() )
			continue;
		
		addedCharsets[ item->qtcodec ] = true;
		menu_enclist->insertItem( item->qtcodec, idx );
	}

	m_MenuView->insertItem( i18n( "&Set codepage"), menu_enclist );
	
	QWhatsThis::whatsThisButton( this );
}

void KCHMSearchAndViewToolbar::setEnabled( bool enable )
{
	m_findBox->setEnabled (enable);
	m_buttonPrev->setEnabled (enable);
	m_buttonNext->setEnabled (enable);
	m_buttonFontInc->setEnabled (enable);
	m_buttonFontDec->setEnabled (enable);
	m_buttonViewSource->setEnabled (enable);
	m_buttonAddBookmark->setEnabled (enable);

	bool enable_toc_nav_buttons = ::mainWindow->contentsWindow() && enable;
	m_buttonNextPageInTOC->setEnabled( enable_toc_nav_buttons );
	m_buttonPrevPageInTOC->setEnabled( enable_toc_nav_buttons );
}

void KCHMSearchAndViewToolbar::onReturnPressed( )
{
	search( true );
}

void KCHMSearchAndViewToolbar::onBtnPrevSearchResult( )
{
	search( false );
}

void KCHMSearchAndViewToolbar::onBtnNextSearchResult( )
{
	search( true );
}

void KCHMSearchAndViewToolbar::search( bool search_forward )
{
	QString searchexpr = m_findBox->lineEdit()->text();

	if ( searchexpr.isEmpty() )
		return;

	::mainWindow->currentBrowser()->searchWord( searchexpr, search_forward, false );
}

void KCHMSearchAndViewToolbar::onBtnFontInc( )
{
	::mainWindow->currentBrowser()->addZoomFactor(1);
}

void KCHMSearchAndViewToolbar::onBtnFontDec( )
{
	::mainWindow->currentBrowser()->addZoomFactor(-1);
}

void KCHMSearchAndViewToolbar::onBtnViewSource( )
{
	QString text;

	if ( !::mainWindow->chmFile()->getFileContentAsString( &text, ::mainWindow->currentBrowser()->getOpenedPage() ) )
		return;

	if ( appConfig.m_advUseInternalEditor )
	{
		QTextEdit * editor = new QTextEdit ( 0 );
		editor->setTextFormat ( Qt::PlainText );
		editor->setText (text);
		editor->setCaption ( QString(APP_NAME) + " - view HTML source of " + ::mainWindow->currentBrowser()->getOpenedPage() );
		editor->resize (800, 600);
		editor->show();
	}
	else
	{
		QFile file;
		m_tempFileKeeper.generateTempFile( file );
		
		file.writeBlock( text.utf8() );
		run_process( appConfig.m_advExternalEditorPath, file.name() );
	}
}

void KCHMSearchAndViewToolbar::onBtnAddBookmark( )
{
	emit ::mainWindow->slotAddBookmark();
}

void KCHMSearchAndViewToolbar::onMenuActivated( int id )
{
	const LCHMTextEncoding * enc = LCHMFileImpl::getTextEncodingTable() + id;
	::mainWindow->setTextEncoding( enc );
}

void KCHMSearchAndViewToolbar::setChosenEncodingInMenu( const LCHMTextEncoding * enc)
{
	if ( m_checkedEncodingInMenu != -1 )
		menu_enclist->setItemChecked( m_checkedEncodingInMenu, false );
	
	if ( m_checkedLanguageInMenu != -1 )
		menu_langlist->setItemChecked( m_checkedLanguageInMenu, false );
	
	int idx = LCHMFileImpl::getEncodingIndex( enc );
	if ( idx == -1 )
		return;
	
	menu_langlist->setItemChecked( idx,  true );
	m_checkedLanguageInMenu = idx;
	
	// For encoding, we need to set up charset!
	const LCHMTextEncoding * enctable = LCHMFileImpl::getTextEncodingTable();
	for ( idx = 0; (enctable + idx)->language; idx++ )
	{
		// See the next item; does is have the same charset as current?
		const LCHMTextEncoding * item = enctable + idx;
	
		// This menu is only for charsets, so we won't add duplicate charset twice
		if ( !strcmp( item->qtcodec, enc->qtcodec ) )
		{
			menu_enclist->setItemChecked ( idx, true);
			m_checkedEncodingInMenu = idx;
			break;
		}
	}
}

void KCHMSearchAndViewToolbar::onBtnNextPageInToc()
{
	KCHMContentsWindow * cwnd = ::mainWindow->contentsWindow();
	
	if ( !cwnd )
		return;
	
	// Try to find current list item
	KCHMIndTocItem * current = cwnd->getTreeItem( ::mainWindow->currentBrowser()->getOpenedPage() );

	if ( !current )
		return;
	
	QListViewItemIterator lit( current );
	lit++;
	
	if ( lit.current() )
	::mainWindow->openPage( ((KCHMIndTocItem *) lit.current() )->getUrl(), OPF_CONTENT_TREE | OPF_ADD2HISTORY );
}

void KCHMSearchAndViewToolbar::onBtnPrevPageInToc()
{
	KCHMContentsWindow * cwnd = ::mainWindow->contentsWindow();
	
	if ( !cwnd )
		return;
	
	// Try to find current list item
	KCHMIndTocItem * current = cwnd->getTreeItem( ::mainWindow->currentBrowser()->getOpenedPage() );
	
	if ( !current )
		return;
	
	QListViewItemIterator lit( current );
	lit--;
	
	if ( lit.current() )
	::mainWindow->openPage( ((KCHMIndTocItem *) lit.current() )->getUrl(), OPF_CONTENT_TREE | OPF_ADD2HISTORY );
}

void KCHMSearchAndViewToolbar::onAccelFocusSearchField( )
{
	m_findBox->setFocus();
}

void KCHMSearchAndViewToolbar::onBtnToggleContentWindow( )
{
	showContentsWindow( !m_MenuView->isItemChecked( m_menuShowContentWindowMenuID ) );
}

void KCHMSearchAndViewToolbar::onBtnFullScreen( )
{
	setFullScreen( !m_MenuView->isItemChecked( m_menuShowFullscreenMenuID ) );
}

void KCHMSearchAndViewToolbar::setFullScreen( bool enable )
{
	::mainWindow->slotEnableFullScreenMode( enable );
	m_MenuView->setItemChecked( m_menuShowFullscreenMenuID, enable ); 
}

void KCHMSearchAndViewToolbar::showContentsWindow( bool enable )
{
	::mainWindow->slotShowContentsWindow( enable );
	m_MenuView->setItemChecked( m_menuShowContentWindowMenuID, enable ); 
}

void KCHMSearchAndViewToolbar::onBtnLocateInContentWindow( )
{
	::mainWindow->slotLocateInContentWindow( );
}

