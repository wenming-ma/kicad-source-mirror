
#ifndef __INCLUDE__CONFIRM_H__
#define __INCLUDE__CONFIRM_H__

#include <kicommon.h>
#include <QString>
#include <QStringList>
#include <functional>

class QWidget;


KICOMMON_API bool AskOverrideLock( QWidget* aParent, const QString& aMessage );


KICOMMON_API bool HandleUnsavedChanges( QWidget* aParent, const QString& aMessage,
                                        const std::function<bool()>& aSaveFunction );


KICOMMON_API int UnsavedChangesDialog( QWidget* aParent, const QString& aMessage,
                                       bool* aApplyToAll );

KICOMMON_API int UnsavedChangesDialog( QWidget* aParent, const QString& aMessage );


KICOMMON_API bool ConfirmRevertDialog( QWidget* parent, const QString& aMessage );


KICOMMON_API void DisplayError( QWidget* aParent, const QString& aText, int aDisplayTime = 0 );

KICOMMON_API void DisplayErrorMessage( QWidget* aParent, const QString& aMessage,
                                       const QString& aExtraInfo = QString() );


KICOMMON_API void DisplayInfoMessage( QWidget* parent, const QString& aMessage,
                                      const QString& aExtraInfo = QString() );

KICOMMON_API bool IsOK( QWidget* aParent, const QString& aMessage );

KICOMMON_API int OKOrCancelDialog( QWidget* aParent, const QString& aWarning,
                                   const QString& aMessage,
                                   const QString& aDetailedMessage = QString(),
                                   const QString& aOKLabel = QString(),
                                   const QString& aCancelLabel = QString(),
                                   bool* aApplyToAll = nullptr );



KICOMMON_API int SelectSingleOption( QWidget* aParent, const QString& aTitle,
                                     const QString& aMessage,
                                     const QStringList& aOptions );

#endif /* __INCLUDE__CONFIRM_H__ */
