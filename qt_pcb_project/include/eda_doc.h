
#ifndef __INCLUDE__EDA_DOC_H__
#define __INCLUDE__EDA_DOC_H__ 1

class EMBEDDED_FILES;

bool GetAssociatedDocument( QWidget* aParent, const QString& aDocName, PROJECT* aProject,
                            SEARCH_STACK* aPaths = nullptr,
                            std::vector<EMBEDDED_FILES*> aFilesStack = {} );


#endif /* __INCLUDE__EDA_DOC_H__ */
