// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-21
#ifndef PROPERTIES_PANEL_H
#define PROPERTIES_PANEL_H

#include <QWidget>
#include <QTreeWidget>
#include <QVariant>
#include <QKeyEvent>
#include <QShowEvent>
#include <QLabel>

#include <vector>
#include <memory>

class EDA_BASE_FRAME;
class EDA_ITEM;
class SELECTION;
class PROPERTY_BASE;

class PROPERTIES_PANEL : public QWidget
{
public:
    PROPERTIES_PANEL( QWidget* aParent, EDA_BASE_FRAME* aFrame );

    virtual ~PROPERTIES_PANEL();

    virtual void UpdateData() = 0;

    virtual void AfterCommit() {}

    QTreeWidget* GetPropertyGrid()
    {
        return m_grid;
    }

    int PropertiesCount() const
    {
        return m_displayed.size();
    }

    const std::vector<PROPERTY_BASE*>& Properties() const
    {
        return m_displayed;
    }

    void RecalculateSplitterPos();

    void SetSplitterProportion( float aProportion );
    float SplitterProportion() const { return m_splitter_key_proportion; }

protected:
    /**
     * Generates the property grid for a given selection of items.
     *
     * @param aSelection is a set of items to show properties for.
     */
    virtual void rebuildProperties( const SELECTION& aSelection );

    virtual QTreeWidgetItem* createPGProperty( const PROPERTY_BASE* aProperty ) const = 0;

    // Event handlers
    virtual void valueChanging( QTreeWidgetItem* aItem ) {}
    virtual void valueChanged( QTreeWidgetItem* aItem ) {}
    void onCharHook( QKeyEvent* aEvent );
    void onShow( QShowEvent* aEvent );

    virtual void OnLanguageChanged( QEvent* aEvent );

    /**
     * Utility to fetch a property value and convert to QVariant
     * Precondition: aItem is known to have property aProperty
     * @return true if conversion succeeded
     */
    bool getItemValue( EDA_ITEM* aItem, PROPERTY_BASE* aProperty, QVariant& aValue );

    /**
     * Processes a selection and determines whether the given property should be available or not
     * and what the common value should be for the items in the selection.
     * @param aSelection is a set of EDA_ITEMs to process
     * @param aProperty is the property to look up
     * @param aValue will be filled with the value common to the selection, or null if different
     * @param aWritable will be set to whether or not the property can be written for the selection
     * @return true if the property is available for all the items in the selection
     */
    bool extractValueAndWritability( const SELECTION& aSelection, PROPERTY_BASE* aProperty,
                                     QVariant& aValue, bool& aWritable );

public:
    int                         m_SuppressGridChangeEvents;

protected:
    std::vector<PROPERTY_BASE*> m_displayed;    // no ownership of pointers
    QTreeWidget*                m_grid;
    EDA_BASE_FRAME*             m_frame;
    QLabel*                     m_caption;

    /// Proportion of the grid column splitter that is used for the key column (0.0 - 1.0)
    float m_splitter_key_proportion;
};

#endif /* PROPERTIES_PANEL_H */
