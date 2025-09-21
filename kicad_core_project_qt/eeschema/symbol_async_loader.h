
#ifndef KICAD_SYMBOL_ASYNC_LOADER_H
#define KICAD_SYMBOL_ASYNC_LOADER_H

#include <atomic>
#include <future>
#include <mutex>
#include <unordered_map>
#include <vector>

#include <QString>

class LIB_SYMBOL;
class PROGRESS_REPORTER;
class SYMBOL_LIB_TABLE;


class SYMBOL_ASYNC_LOADER
{
public:
    /**
     * Construct a loader for symbol libraries.
     *
     * @param aNicknames is a list of library nicknames to load.
     * @param aTable is a pointer to the symbol library table to load libraries for.
     * @param aOnlyPowerSymbols, if true, will only return power symbols in the output map.
     * @param aOutput will be filled with the loaded parts.
     * @param aReporter will be used to report progress, of not null.
     */
    SYMBOL_ASYNC_LOADER( const std::vector<QString>& aNicknames,
                         SYMBOL_LIB_TABLE* aTable, bool aOnlyPowerSymbols = false,
                         std::unordered_map<QString, std::vector<LIB_SYMBOL*>>* aOutput = nullptr,
                         PROGRESS_REPORTER* aReporter = nullptr );

    ~SYMBOL_ASYNC_LOADER();

    /**
     * Spin up threads to load all the libraries in m_nicknames.
     */
    void Start();

    /**
     * Finalize the threads and combines the output into the target output map.
     */
    bool Join();

    /// @return true if loading is done
    bool Done();

    /// @return a string containing any errors generated during the load.
    const QString& GetErrors() const { return m_errors; }

    /// Represent a pair of <nickname, loaded parts list>.
    typedef std::pair<QString, std::vector<LIB_SYMBOL*>> LOADED_PAIR;

private:
    /// Worker job that loads libraries and returns a list of pairs of <nickname, loaded parts>.
    std::vector<LOADED_PAIR> worker();

    /// List of libraries to load.
    std::vector<QString> m_nicknames;

    /// Handle to the symbol library table being loaded into.
    SYMBOL_LIB_TABLE* m_table;

    /// True if we are loading only power symbols.
    bool m_onlyPowerSymbols;

    /// Handle to map that will be filled with the loaded parts per library.
    std::unordered_map<QString, std::vector<LIB_SYMBOL*>>* m_output;

    /// Progress reporter (may be null).
    PROGRESS_REPORTER* m_reporter;

    size_t              m_threadCount;
    std::atomic<size_t> m_nextLibrary;
    QString             m_errors;
    std::mutex          m_errorMutex;

    std::vector<std::future<std::vector<LOADED_PAIR>>> m_returns;
};

#endif
