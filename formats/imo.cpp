#include "imo.h"


/**
 * Constructor
 */
Imo::Imo()
{
    final = new StdFormat();
    final->setClient("Meebo");
}

/**
 * Load a Chat Log
 */
void Imo::load(QVariant $log_raw)
{
    ;
}

/**
 * Generate Log from Standardized Log
 */
QVariant Imo::generate(StdFormat *$log)
{
    ;
}

/**
 * Process "From" Request
 */
StdFormat* Imo::from(QHash<QString, QVariant> data)
{
    ;
}

/**
 * Process "To" Request
 */
void Imo::to(StdFormat* $log)
{
    ;
}
