package pl.edu.mimuw.seega

import pl.edu.mimuw.seega.domain.Board
import pl.edu.mimuw.seega.domain.Direction
import pl.edu.mimuw.seega.domain.PawnColor
import pl.edu.mimuw.seega.exceptions.FieldEmptinessException
import pl.edu.mimuw.seega.exceptions.FieldOutOfBoundsException
import pl.edu.mimuw.seega.exceptions.FieldPlayerMismatchException

class SeegaController(private val board: Board) {
    var movesWithoutTaking: Int = 0
        private set
    var currentPlayerColor: PawnColor = PawnColor.WHITE
        private set

    fun executeDeploy(col: Char, row: Int) {
        if (!board.isFieldInBounds(col, row))
            throw FieldOutOfBoundsException("Field is out of bounds.")
        if (!board.isFieldEmpty(col, row))
            throw FieldEmptinessException("Field is not empty.")
        board.placePawn(col, row, currentPlayerColor)
    }

    fun executeMove(col: Char, row: Int, direction: Direction): Boolean {
        if (!board.isFieldInBounds(col, row))
            throw FieldOutOfBoundsException("Field is out of bounds.")
        if (!board.isFieldInBounds(col + direction.col, row + direction.row))
            throw FieldOutOfBoundsException("Desired field is out of bounds.")
        if (board.isFieldEmpty(col, row))
            throw FieldEmptinessException("Field is empty.")
        if (!board.isFieldEmpty(col + direction.col, row + direction.row))
            throw FieldEmptinessException("Desired field is not empty.")
        if (board.getFieldColor(col, row) != currentPlayerColor)
            throw FieldPlayerMismatchException("Field is not yours.")

        board.movePawnAndGetNewField(col, row, direction).also {
            val (newCol, newRow) = it
            val pawnsTaken = board.takeOpponentPawnsAndGetResult(newCol, newRow)
            if (pawnsTaken) movesWithoutTaking = 0 else movesWithoutTaking++
            return pawnsTaken
        }
    }

    fun proceedToNextPhase() {
        board.removePawn('a' + board.size / 2, board.size / 2 + 1)
    }

    fun isPhaseOne(): Boolean = board.size * board.size > board.whitePawns + board.blackPawns + 1

    fun isPhaseTwo(): Boolean = board.whitePawns > 0 && board.blackPawns > 0 && movesWithoutTaking < 20

    fun whoWon(): PawnColor =
        if (board.whitePawns == 0) PawnColor.BLACK else if (board.blackPawns == 0) PawnColor.WHITE else PawnColor.EMPTY

    fun changeColor() {
        currentPlayerColor = PawnColor.getOppositeColor(currentPlayerColor)
    }
}