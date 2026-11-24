% politicas.pl
%
% Pares de cursos incompatibles (Regla 3 de la Etapa 3). Cada hecho indica
% que un estudiante no puede llevar ambos cursos en la misma combinación.
% La relación es simétrica: la regla que la aplique (Persona 2) debe
% comprobar ambos sentidos.
%
% PROPUESTA: estos dos pares se eligieron a partir de los planes de estudio
% del catálogo y deben verificarse contra el plan oficial antes de entregar.
%   - MA0101 (Matemática General) es el curso de nivelación que no se lleva
%     junto con el primer cálculo, MA1102.
%   - MA1102 y MA1103 son dos cursos de cálculo alternativos; el plan
%     solo admite uno de los dos.

incompatible('MA0101', 'MA1102').
incompatible('MA1102', 'MA1103').
