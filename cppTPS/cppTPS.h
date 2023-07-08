#pragma once

#include <iostream>
#include <vector>
#include <string>
using namespace std;
#include "cppTPS_Transaction.h"

class cppTPS {
private:
    vector < cppTPS_Transaction * >transactions;
    int numTransactions;
    int mostRecentTransaction;
    bool performingDo;
    bool performingUndo;
    
    // HELPER FUNCTION
    void popTopTransaction () {
        cppTPS_Transaction * t = this->transactions[this->numTransactions - 1];
        this->transactions.pop_back ();
        delete t;
        this->numTransactions--;
    } 
 
public:
    // INLINED FUNCTIONS
    bool isPerformingDo () {
        return this->performingDo;
    }
    
    bool isPerformingUndo () {
        return this->performingUndo;
    }
    
    bool hasTransactionToRedo () {
        return (this->mostRecentTransaction + 1) < this->numTransactions;
    }

    bool hasTransactionToUndo () {
        return this->mostRecentTransaction >= 0;
    }
    
    int getSize () {
        return (int) this->transactions.size ();
    }
    
    int getRedoSize () {
        return (int) this->getSize () - this->mostRecentTransaction - 1;
    }
    
    int getUndoSize () {
        return this->mostRecentTransaction + 1;
    }
    
    /*
     * Default constructor for the transaction processing system.
     */ 
    cppTPS () {
        this->numTransactions = 0;
        this->mostRecentTransaction = -1;
        this->performingDo = false;
        this->performingUndo = false;
    }
  
    /*
     * Adds a new transaction to the stack
     */
     void addTransaction (cppTPS_Transaction * transaction) {
         // ARE WE BRANCHING? IF WE ARE WE NEED TO DELETE ALL THE OLD TRANSACTIONS
         while (this->hasTransactionToRedo ()) {
             this->popTopTransaction ();
         }
         this->numTransactions++;
         
         // ADD THE TRANSACTION
         this->transactions.push_back (transaction);
         
         // AND EXECUTE IT
         this->doTransaction ();
     }
 
    /*
     * Does the transaction currently at the top of the stack. Note, it might
     * be a redo.
     */ 
     void doTransaction () {
         if (this->hasTransactionToRedo ()) {
             this->performingDo = true;
             cppTPS_Transaction * transaction = this->transactions[this->mostRecentTransaction + 1];
             transaction->doTransaction ();
             this->mostRecentTransaction++;
             this->performingDo = false;
         }
     }
 
    /**
     * Gets the most recently executed transaction on the
     * TPS stack and undoes it, moving the TPS counter accordingly.
     */
     void undoTransaction () {
         if (this->hasTransactionToUndo ()) {
             this->performingUndo = true;
             cppTPS_Transaction * transaction = this->transactions[this->mostRecentTransaction];
             transaction->undoTransaction ();
             this->mostRecentTransaction--;
             this->performingUndo = false;
         }
     }
 
    /*
     * Clears out all of the transactions on the stack.
     */ 
     void clearAllTransactions () {
         // FIRST DELETE ALL THE TRANSACTIONS, WE NEED TO PREVENT A MEMORY LEAK
         for (int i = this->numTransactions - 1; i >= 0; i--) {
             this->popTopTransaction ();
         } 
         
         // MAKE SURE TO RESET THE LOCATION OF THE
         // TOP OF THE TPS STACK TOO
         this->mostRecentTransaction = -1;
     } 
 
    /*
     * Returns a textual representation of the transaction stack.
     */
    wstring toString () {
        wstringstream wss;
        wss << "--Number of Transactions: " << this->numTransactions << '\n';
        wss << "--Current Index on Stack: " << this->mostRecentTransaction << '\n';
        wss << L"--Current Transaction Stack:" << '\n';
        for (int i = 0; i <= this->mostRecentTransaction; i++) {
            cppTPS_Transaction * cppT = this->transactions[i];
            wss << "----" << cppT->toString () << '\n';
        } 
        wstring text = wss.str ();
        return text;
    }
};