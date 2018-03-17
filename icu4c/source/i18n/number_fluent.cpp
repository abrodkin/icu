// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#include <unicode/numberformatter.h>
#include "unicode/utypes.h"

#if !UCONFIG_NO_FORMATTING && !UPRV_INCOMPLETE_CPP11_SUPPORT

#include "uassert.h"
#include "unicode/numberformatter.h"
#include "number_decimalquantity.h"
#include "number_formatimpl.h"
#include "umutex.h"

using namespace icu;
using namespace icu::number;
using namespace icu::number::impl;

template<typename Derived>
Derived NumberFormatterSettings<Derived>::notation(const Notation& notation) const & {
    Derived copy(*this);
    // NOTE: Slicing is OK.
    copy.fMacros.notation = notation;
    return copy;
}

template<typename Derived>
Derived NumberFormatterSettings<Derived>::notation(const Notation& notation) && {
    Derived move(std::move(*this));
    // NOTE: Slicing is OK.
    move.fMacros.notation = notation;
    return move;
}

template<typename Derived>
Derived NumberFormatterSettings<Derived>::unit(const icu::MeasureUnit& unit) const & {
    Derived copy(*this);
    // NOTE: Slicing occurs here. However, CurrencyUnit can be restored from MeasureUnit.
    // TimeUnit may be affected, but TimeUnit is not as relevant to number formatting.
    copy.fMacros.unit = unit;
    return copy;
}

template<typename Derived>
Derived NumberFormatterSettings<Derived>::unit(const icu::MeasureUnit& unit) && {
    Derived move(std::move(*this));
    // See comments above about slicing.
    move.fMacros.unit = unit;
    return move;
}

template<typename Derived>
Derived NumberFormatterSettings<Derived>::adoptUnit(icu::MeasureUnit* unit) const & {
    Derived copy(*this);
    // Just move the unit into the MacroProps by value, and delete it since we have ownership.
    // NOTE: Slicing occurs here. However, CurrencyUnit can be restored from MeasureUnit.
    // TimeUnit may be affected, but TimeUnit is not as relevant to number formatting.
    if (unit != nullptr) {
        // TODO: On nullptr, reset to default value?
        copy.fMacros.unit = std::move(*unit);
        delete unit;
    }
    return copy;
}

template<typename Derived>
Derived NumberFormatterSettings<Derived>::adoptUnit(icu::MeasureUnit* unit) && {
    Derived move(std::move(*this));
    // See comments above about slicing and ownership.
    if (unit != nullptr) {
        // TODO: On nullptr, reset to default value?
        move.fMacros.unit = std::move(*unit);
        delete unit;
    }
    return move;
}

template<typename Derived>
Derived NumberFormatterSettings<Derived>::perUnit(const icu::MeasureUnit& perUnit) const & {
    Derived copy(*this);
    // See comments above about slicing.
    copy.fMacros.perUnit = perUnit;
    return copy;
}

template<typename Derived>
Derived NumberFormatterSettings<Derived>::perUnit(const icu::MeasureUnit& perUnit) && {
    Derived copy(*this);
    // See comments above about slicing.
    copy.fMacros.perUnit = perUnit;
    return copy;
}

template<typename Derived>
Derived NumberFormatterSettings<Derived>::adoptPerUnit(icu::MeasureUnit* perUnit) const & {
    Derived move(std::move(*this));
    // See comments above about slicing and ownership.
    if (perUnit != nullptr) {
        // TODO: On nullptr, reset to default value?
        move.fMacros.perUnit = std::move(*perUnit);
        delete perUnit;
    }
    return move;
}

template<typename Derived>
Derived NumberFormatterSettings<Derived>::adoptPerUnit(icu::MeasureUnit* perUnit) && {
    Derived copy(*this);
    // See comments above about slicing and ownership.
    if (perUnit != nullptr) {
        // TODO: On nullptr, reset to default value?
        copy.fMacros.perUnit = std::move(*perUnit);
        delete perUnit;
    }
    return copy;
}

template<typename Derived>
Derived NumberFormatterSettings<Derived>::rounding(const Rounder& rounder) const & {
    Derived copy(*this);
    // NOTE: Slicing is OK.
    copy.fMacros.rounder = rounder;
    return copy;
}

template<typename Derived>
Derived NumberFormatterSettings<Derived>::rounding(const Rounder& rounder) && {
    Derived move(std::move(*this));
    // NOTE: Slicing is OK.
    move.fMacros.rounder = rounder;
    return move;
}

template<typename Derived>
Derived NumberFormatterSettings<Derived>::grouping(const UGroupingStrategy& strategy) const & {
    Derived copy(*this);
    // NOTE: This is slightly different than how the setting is stored in Java
    // because we want to put it on the stack.
    copy.fMacros.grouper = Grouper::forStrategy(strategy);
    return copy;
}

template<typename Derived>
Derived NumberFormatterSettings<Derived>::grouping(const UGroupingStrategy& strategy) && {
    Derived move(std::move(*this));
    move.fMacros.grouper = Grouper::forStrategy(strategy);
    return move;
}

template<typename Derived>
Derived NumberFormatterSettings<Derived>::integerWidth(const IntegerWidth& style) const & {
    Derived copy(*this);
    copy.fMacros.integerWidth = style;
    return copy;
}

template<typename Derived>
Derived NumberFormatterSettings<Derived>::integerWidth(const IntegerWidth& style) && {
    Derived move(std::move(*this));
    move.fMacros.integerWidth = style;
    return move;
}

template<typename Derived>
Derived NumberFormatterSettings<Derived>::symbols(const DecimalFormatSymbols& symbols) const & {
    Derived copy(*this);
    copy.fMacros.symbols.setTo(symbols);
    return copy;
}

template<typename Derived>
Derived NumberFormatterSettings<Derived>::symbols(const DecimalFormatSymbols& symbols) && {
    Derived move(std::move(*this));
    move.fMacros.symbols.setTo(symbols);
    return move;
}

template<typename Derived>
Derived NumberFormatterSettings<Derived>::adoptSymbols(NumberingSystem* ns) const & {
    Derived copy(*this);
    copy.fMacros.symbols.setTo(ns);
    return copy;
}

template<typename Derived>
Derived NumberFormatterSettings<Derived>::adoptSymbols(NumberingSystem* ns) && {
    Derived move(std::move(*this));
    move.fMacros.symbols.setTo(ns);
    return move;
}

template<typename Derived>
Derived NumberFormatterSettings<Derived>::unitWidth(const UNumberUnitWidth& width) const & {
    Derived copy(*this);
    copy.fMacros.unitWidth = width;
    return copy;
}

template<typename Derived>
Derived NumberFormatterSettings<Derived>::unitWidth(const UNumberUnitWidth& width) && {
    Derived move(std::move(*this));
    move.fMacros.unitWidth = width;
    return move;
}

template<typename Derived>
Derived NumberFormatterSettings<Derived>::sign(const UNumberSignDisplay& style) const & {
    Derived copy(*this);
    copy.fMacros.sign = style;
    return copy;
}

template<typename Derived>
Derived NumberFormatterSettings<Derived>::sign(const UNumberSignDisplay& style) && {
    Derived move(std::move(*this));
    move.fMacros.sign = style;
    return move;
}

template<typename Derived>
Derived NumberFormatterSettings<Derived>::decimal(const UNumberDecimalSeparatorDisplay& style) const & {
    Derived copy(*this);
    copy.fMacros.decimal = style;
    return copy;
}

template<typename Derived>
Derived NumberFormatterSettings<Derived>::decimal(const UNumberDecimalSeparatorDisplay& style) && {
    Derived move(std::move(*this));
    move.fMacros.decimal = style;
    return move;
}

template<typename Derived>
Derived NumberFormatterSettings<Derived>::padding(const Padder& padder) const & {
    Derived copy(*this);
    copy.fMacros.padder = padder;
    return copy;
}

template<typename Derived>
Derived NumberFormatterSettings<Derived>::padding(const Padder& padder) && {
    Derived move(std::move(*this));
    move.fMacros.padder = padder;
    return move;
}

template<typename Derived>
Derived NumberFormatterSettings<Derived>::threshold(int32_t threshold) const & {
    Derived copy(*this);
    copy.fMacros.threshold = threshold;
    return copy;
}

template<typename Derived>
Derived NumberFormatterSettings<Derived>::threshold(int32_t threshold) && {
    Derived move(std::move(*this));
    move.fMacros.threshold = threshold;
    return move;
}

template<typename Derived>
Derived NumberFormatterSettings<Derived>::macros(const impl::MacroProps& macros) const & {
    Derived copy(*this);
    copy.fMacros = macros;
    return copy;
}

template<typename Derived>
Derived NumberFormatterSettings<Derived>::macros(const impl::MacroProps& macros) && {
    Derived move(std::move(*this));
    move.fMacros = macros;
    return move;
}

template<typename Derived>
Derived NumberFormatterSettings<Derived>::macros(impl::MacroProps&& macros) const & {
    Derived copy(*this);
    copy.fMacros = std::move(macros);
    return copy;
}

template<typename Derived>
Derived NumberFormatterSettings<Derived>::macros(impl::MacroProps&& macros) && {
    Derived move(std::move(*this));
    move.fMacros = std::move(macros);
    return move;
}

// Declare all classes that implement NumberFormatterSettings
// See https://stackoverflow.com/a/495056/1407170
template
class icu::number::NumberFormatterSettings<icu::number::UnlocalizedNumberFormatter>;
template
class icu::number::NumberFormatterSettings<icu::number::LocalizedNumberFormatter>;


UnlocalizedNumberFormatter NumberFormatter::with() {
    UnlocalizedNumberFormatter result;
    return result;
}

LocalizedNumberFormatter NumberFormatter::withLocale(const Locale& locale) {
    return with().locale(locale);
}


template<typename T>
using NFS = NumberFormatterSettings<T>;
using LNF = LocalizedNumberFormatter;
using UNF = UnlocalizedNumberFormatter;

UnlocalizedNumberFormatter::UnlocalizedNumberFormatter(const UNF& other)
        : UNF(static_cast<const NFS<UNF>&>(other)) {}

UnlocalizedNumberFormatter::UnlocalizedNumberFormatter(const NFS<UNF>& other)
        : NFS<UNF>(other) {
    // No additional fields to assign
}

UnlocalizedNumberFormatter::UnlocalizedNumberFormatter(UNF&& src) U_NOEXCEPT
        : UNF(static_cast<NFS<UNF>&&>(src)) {}

UnlocalizedNumberFormatter::UnlocalizedNumberFormatter(NFS<UNF>&& src) U_NOEXCEPT
        : NFS<UNF>(std::move(src)) {
    // No additional fields to assign
}

UnlocalizedNumberFormatter& UnlocalizedNumberFormatter::operator=(const UNF& other) {
    NFS<UNF>::operator=(static_cast<const NFS<UNF>&>(other));
    // No additional fields to assign
    return *this;
}

UnlocalizedNumberFormatter& UnlocalizedNumberFormatter::operator=(UNF&& src) U_NOEXCEPT {
    NFS<UNF>::operator=(static_cast<NFS<UNF>&&>(src));
    // No additional fields to assign
    return *this;
}

LocalizedNumberFormatter::LocalizedNumberFormatter(const LNF& other)
        : LNF(static_cast<const NFS<LNF>&>(other)) {}

LocalizedNumberFormatter::LocalizedNumberFormatter(const NFS<LNF>& other)
        : NFS<LNF>(other) {
    // No additional fields to assign (let call count and compiled formatter reset to defaults)
}

LocalizedNumberFormatter::LocalizedNumberFormatter(LocalizedNumberFormatter&& src) U_NOEXCEPT
        : LNF(static_cast<NFS<LNF>&&>(src)) {}

LocalizedNumberFormatter::LocalizedNumberFormatter(NFS<LNF>&& src) U_NOEXCEPT
        : NFS<LNF>(std::move(src)) {
    // For the move operators, copy over the call count and compiled formatter.
    auto&& srcAsLNF = static_cast<LNF&&>(src);
    fCompiled = srcAsLNF.fCompiled;
    uprv_memcpy(fUnsafeCallCount, srcAsLNF.fUnsafeCallCount, sizeof(fUnsafeCallCount));
    // Reset the source object to leave it in a safe state.
    srcAsLNF.fCompiled = nullptr;
    uprv_memset(srcAsLNF.fUnsafeCallCount, 0, sizeof(fUnsafeCallCount));
}

LocalizedNumberFormatter& LocalizedNumberFormatter::operator=(const LNF& other) {
    NFS<LNF>::operator=(static_cast<const NFS<LNF>&>(other));
    // No additional fields to assign (let call count and compiled formatter reset to defaults)
    return *this;
}

LocalizedNumberFormatter& LocalizedNumberFormatter::operator=(LNF&& src) U_NOEXCEPT {
    NFS<LNF>::operator=(static_cast<NFS<LNF>&&>(src));
    // For the move operators, copy over the call count and compiled formatter.
    fCompiled = src.fCompiled;
    uprv_memcpy(fUnsafeCallCount, src.fUnsafeCallCount, sizeof(fUnsafeCallCount));
    // Reset the source object to leave it in a safe state.
    src.fCompiled = nullptr;
    uprv_memset(src.fUnsafeCallCount, 0, sizeof(fUnsafeCallCount));
    return *this;
}


LocalizedNumberFormatter::~LocalizedNumberFormatter() {
    delete fCompiled;
}

LocalizedNumberFormatter::LocalizedNumberFormatter(const MacroProps& macros, const Locale& locale) {
    fMacros = macros;
    fMacros.locale = locale;
}

LocalizedNumberFormatter::LocalizedNumberFormatter(MacroProps&& macros, const Locale& locale) {
    fMacros = std::move(macros);
    fMacros.locale = locale;
}

LocalizedNumberFormatter UnlocalizedNumberFormatter::locale(const Locale& locale) const & {
    return LocalizedNumberFormatter(fMacros, locale);
}

LocalizedNumberFormatter UnlocalizedNumberFormatter::locale(const Locale& locale) && {
    return LocalizedNumberFormatter(std::move(fMacros), locale);
}

SymbolsWrapper::SymbolsWrapper(const SymbolsWrapper& other) {
    doCopyFrom(other);
}

SymbolsWrapper::SymbolsWrapper(SymbolsWrapper&& src) U_NOEXCEPT {
    doMoveFrom(std::move(src));
}

SymbolsWrapper& SymbolsWrapper::operator=(const SymbolsWrapper& other) {
    if (this == &other) {
        return *this;
    }
    doCleanup();
    doCopyFrom(other);
    return *this;
}

SymbolsWrapper& SymbolsWrapper::operator=(SymbolsWrapper&& src) U_NOEXCEPT {
    if (this == &src) {
        return *this;
    }
    doCleanup();
    doMoveFrom(std::move(src));
    return *this;
}

SymbolsWrapper::~SymbolsWrapper() {
    doCleanup();
}

void SymbolsWrapper::setTo(const DecimalFormatSymbols& dfs) {
    doCleanup();
    fType = SYMPTR_DFS;
    fPtr.dfs = new DecimalFormatSymbols(dfs);
}

void SymbolsWrapper::setTo(const NumberingSystem* ns) {
    doCleanup();
    fType = SYMPTR_NS;
    fPtr.ns = ns;
}

void SymbolsWrapper::doCopyFrom(const SymbolsWrapper& other) {
    fType = other.fType;
    switch (fType) {
        case SYMPTR_NONE:
            // No action necessary
            break;
        case SYMPTR_DFS:
            // Memory allocation failures are exposed in copyErrorTo()
            if (other.fPtr.dfs != nullptr) {
                fPtr.dfs = new DecimalFormatSymbols(*other.fPtr.dfs);
            } else {
                fPtr.dfs = nullptr;
            }
            break;
        case SYMPTR_NS:
            // Memory allocation failures are exposed in copyErrorTo()
            if (other.fPtr.ns != nullptr) {
                fPtr.ns = new NumberingSystem(*other.fPtr.ns);
            } else {
                fPtr.ns = nullptr;
            }
            break;
    }
}

void SymbolsWrapper::doMoveFrom(SymbolsWrapper&& src) {
    fType = src.fType;
    switch (fType) {
        case SYMPTR_NONE:
            // No action necessary
            break;
        case SYMPTR_DFS:
            fPtr.dfs = src.fPtr.dfs;
            src.fPtr.dfs = nullptr;
            break;
        case SYMPTR_NS:
            fPtr.ns = src.fPtr.ns;
            src.fPtr.ns = nullptr;
            break;
    }
}

void SymbolsWrapper::doCleanup() {
    switch (fType) {
        case SYMPTR_NONE:
            // No action necessary
            break;
        case SYMPTR_DFS:
            delete fPtr.dfs;
            break;
        case SYMPTR_NS:
            delete fPtr.ns;
            break;
    }
}

bool SymbolsWrapper::isDecimalFormatSymbols() const {
    return fType == SYMPTR_DFS;
}

bool SymbolsWrapper::isNumberingSystem() const {
    return fType == SYMPTR_NS;
}

const DecimalFormatSymbols* SymbolsWrapper::getDecimalFormatSymbols() const {
    U_ASSERT(fType == SYMPTR_DFS);
    return fPtr.dfs;
}

const NumberingSystem* SymbolsWrapper::getNumberingSystem() const {
    U_ASSERT(fType == SYMPTR_NS);
    return fPtr.ns;
}


FormattedNumber::FormattedNumber(FormattedNumber&& src) U_NOEXCEPT
        : fResults(src.fResults), fErrorCode(src.fErrorCode) {
    // Disown src.fResults to prevent double-deletion
    src.fResults = nullptr;
    src.fErrorCode = U_INVALID_STATE_ERROR;
}

FormattedNumber& FormattedNumber::operator=(FormattedNumber&& src) U_NOEXCEPT {
    delete fResults;
    fResults = src.fResults;
    fErrorCode = src.fErrorCode;
    // Disown src.fResults to prevent double-deletion
    src.fResults = nullptr;
    src.fErrorCode = U_INVALID_STATE_ERROR;
    return *this;
}

FormattedNumber LocalizedNumberFormatter::formatInt(int64_t value, UErrorCode& status) const {
    if (U_FAILURE(status)) { return FormattedNumber(U_ILLEGAL_ARGUMENT_ERROR); }
    auto results = new NumberFormatterResults();
    if (results == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        return FormattedNumber(status);
    }
    results->quantity.setToLong(value);
    return formatImpl(results, status);
}

FormattedNumber LocalizedNumberFormatter::formatDouble(double value, UErrorCode& status) const {
    if (U_FAILURE(status)) { return FormattedNumber(U_ILLEGAL_ARGUMENT_ERROR); }
    auto results = new NumberFormatterResults();
    if (results == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        return FormattedNumber(status);
    }
    results->quantity.setToDouble(value);
    return formatImpl(results, status);
}

FormattedNumber LocalizedNumberFormatter::formatDecimal(StringPiece value, UErrorCode& status) const {
    if (U_FAILURE(status)) { return FormattedNumber(U_ILLEGAL_ARGUMENT_ERROR); }
    auto results = new NumberFormatterResults();
    if (results == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        return FormattedNumber(status);
    }
    results->quantity.setToDecNumber(value);
    return formatImpl(results, status);
}

FormattedNumber
LocalizedNumberFormatter::formatDecimalQuantity(const DecimalQuantity& dq, UErrorCode& status) const {
    if (U_FAILURE(status)) { return FormattedNumber(U_ILLEGAL_ARGUMENT_ERROR); }
    auto results = new NumberFormatterResults();
    if (results == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        return FormattedNumber(status);
    }
    results->quantity = dq;
    return formatImpl(results, status);
}

FormattedNumber
LocalizedNumberFormatter::formatImpl(impl::NumberFormatterResults* results, UErrorCode& status) const {
    // fUnsafeCallCount contains memory to be interpreted as an atomic int, most commonly
    // std::atomic<int32_t>.  Since the type of atomic int is platform-dependent, we cast the
    // bytes in fUnsafeCallCount to u_atomic_int32_t, a typedef for the platform-dependent
    // atomic int type defined in umutex.h.
    static_assert(
            sizeof(u_atomic_int32_t) <= sizeof(fUnsafeCallCount),
            "Atomic integer size on this platform exceeds the size allocated by fUnsafeCallCount");
    auto* callCount = reinterpret_cast<u_atomic_int32_t*>(
            const_cast<LocalizedNumberFormatter*>(this)->fUnsafeCallCount);

    // A positive value in the atomic int indicates that the data structure is not yet ready;
    // a negative value indicates that it is ready. If, after the increment, the atomic int
    // is exactly threshold, then it is the current thread's job to build the data structure.
    // Note: We set the callCount to INT32_MIN so that if another thread proceeds to increment
    // the atomic int, the value remains below zero.
    int32_t currentCount = umtx_loadAcquire(*callCount);
    if (0 <= currentCount && currentCount <= fMacros.threshold && fMacros.threshold > 0) {
        currentCount = umtx_atomic_inc(callCount);
    }

    if (currentCount == fMacros.threshold && fMacros.threshold > 0) {
        // Build the data structure and then use it (slow to fast path).
        const NumberFormatterImpl* compiled = NumberFormatterImpl::fromMacros(fMacros, status);
        U_ASSERT(fCompiled == nullptr);
        const_cast<LocalizedNumberFormatter*>(this)->fCompiled = compiled;
        umtx_storeRelease(*callCount, INT32_MIN);
        compiled->apply(results->quantity, results->string, status);
    } else if (currentCount < 0) {
        // The data structure is already built; use it (fast path).
        U_ASSERT(fCompiled != nullptr);
        fCompiled->apply(results->quantity, results->string, status);
    } else {
        // Format the number without building the data structure (slow path).
        NumberFormatterImpl::applyStatic(fMacros, results->quantity, results->string, status);
    }

    // Do not save the results object if we encountered a failure.
    if (U_SUCCESS(status)) {
        return FormattedNumber(results);
    } else {
        delete results;
        return FormattedNumber(status);
    }
}

const impl::NumberFormatterImpl* LocalizedNumberFormatter::getCompiled() const {
    return fCompiled;
}

int32_t LocalizedNumberFormatter::getCallCount() const {
    auto* callCount = reinterpret_cast<u_atomic_int32_t*>(
            const_cast<LocalizedNumberFormatter*>(this)->fUnsafeCallCount);
    return umtx_loadAcquire(*callCount);
}

UnicodeString FormattedNumber::toString() const {
    if (fResults == nullptr) {
        // TODO: http://bugs.icu-project.org/trac/ticket/13437
        return {};
    }
    return fResults->string.toUnicodeString();
}

Appendable& FormattedNumber::appendTo(Appendable& appendable) {
    if (fResults == nullptr) {
        // TODO: http://bugs.icu-project.org/trac/ticket/13437
        return appendable;
    }
    appendable.appendString(fResults->string.chars(), fResults->string.length());
    return appendable;
}

void FormattedNumber::populateFieldPosition(FieldPosition& fieldPosition, UErrorCode& status) {
    if (U_FAILURE(status)) { return; }
    if (fResults == nullptr) {
        status = fErrorCode;
        return;
    }
    fResults->string.populateFieldPosition(fieldPosition, 0, status);
}

void FormattedNumber::populateFieldPositionIterator(FieldPositionIterator& iterator, UErrorCode& status) {
    if (U_FAILURE(status)) { return; }
    if (fResults == nullptr) {
        status = fErrorCode;
        return;
    }
    fResults->string.populateFieldPositionIterator(iterator, status);
}

void FormattedNumber::getDecimalQuantity(DecimalQuantity& output, UErrorCode& status) const {
    if (U_FAILURE(status)) { return; }
    if (fResults == nullptr) {
        status = fErrorCode;
        return;
    }
    output = fResults->quantity;
}

const UnicodeString FormattedNumber::getPrefix(UErrorCode& status) const {
    if (fResults == nullptr) {
        status = fErrorCode;
        return {};
    }
    // FIXME
    return {};
}

const UnicodeString FormattedNumber::getSuffix(UErrorCode& status) const {
    if (fResults == nullptr) {
        status = fErrorCode;
        return {};
    }
    // FIXME
    return {};
}

FormattedNumber::~FormattedNumber() {
    delete fResults;
}

#endif /* #if !UCONFIG_NO_FORMATTING */
